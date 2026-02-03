#include "ocgd_iges_reader.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>

// OpenCASCADE includes
#include <IGESCAFControl_Reader.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <TDF_LabelSequence.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <IGESControl_Reader.hxx>
#include <Interface_Static.hxx>
#include <Standard_Failure.hxx>

using namespace godot;

ocgd_iges_reader::ocgd_iges_reader() {
    transfer_colors = true;
    transfer_names = true;
    transfer_layers = true;
    precision = 0.01;
    last_error = "";
}

ocgd_iges_reader::~ocgd_iges_reader() {
}

void ocgd_iges_reader::_bind_methods() {
    ClassDB::bind_static_method("ocgd_iges_reader", D_METHOD("new_reader"), &ocgd_iges_reader::new_reader);
    
    ClassDB::bind_method(D_METHOD("load_file", "file_path"), &ocgd_iges_reader::load_file);
    ClassDB::bind_method(D_METHOD("load_file_with_options", "file_path", "options"), &ocgd_iges_reader::load_file_with_options);
    ClassDB::bind_method(D_METHOD("get_file_info"), &ocgd_iges_reader::get_file_info);
    ClassDB::bind_method(D_METHOD("get_all_shapes"), &ocgd_iges_reader::get_all_shapes);
    
    ClassDB::bind_method(D_METHOD("set_transfer_colors", "enable"), &ocgd_iges_reader::set_transfer_colors);
    ClassDB::bind_method(D_METHOD("get_transfer_colors"), &ocgd_iges_reader::get_transfer_colors);
    ClassDB::bind_method(D_METHOD("set_transfer_names", "enable"), &ocgd_iges_reader::set_transfer_names);
    ClassDB::bind_method(D_METHOD("get_transfer_names"), &ocgd_iges_reader::get_transfer_names);
    ClassDB::bind_method(D_METHOD("set_transfer_layers", "enable"), &ocgd_iges_reader::set_transfer_layers);
    ClassDB::bind_method(D_METHOD("get_transfer_layers"), &ocgd_iges_reader::get_transfer_layers);
    ClassDB::bind_method(D_METHOD("set_precision", "precision"), &ocgd_iges_reader::set_precision);
    ClassDB::bind_method(D_METHOD("get_precision"), &ocgd_iges_reader::get_precision);
    
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_iges_reader::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_iges_reader::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_iges_reader::clear_error);
    
    ClassDB::bind_method(D_METHOD("validate_file", "file_path"), &ocgd_iges_reader::validate_file);
    ClassDB::bind_method(D_METHOD("get_supported_extensions"), &ocgd_iges_reader::get_supported_extensions);
    ClassDB::bind_method(D_METHOD("get_entity_count"), &ocgd_iges_reader::get_entity_count);
    ClassDB::bind_method(D_METHOD("get_file_statistics"), &ocgd_iges_reader::get_file_statistics);
    
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_iges_reader::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_iges_reader::is_null);
}

Ref<ocgd_iges_reader> ocgd_iges_reader::new_reader() {
    return memnew(ocgd_iges_reader);
}

Ref<ocgd_shape> ocgd_iges_reader::load_file(const String& file_path) {
    Dictionary options;
    return load_file_with_options(file_path, options);
}

Ref<ocgd_shape> ocgd_iges_reader::load_file_with_options(const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), Ref<ocgd_shape>(), "File path is empty");
    
    try {
        // Create XCAF document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Create IGES reader
        IGESCAFControl_Reader reader;
        
        // Configure reader based on options and properties
        reader.SetColorMode(transfer_colors);
        reader.SetNameMode(transfer_names);
        reader.SetLayerMode(transfer_layers);
        
        // Set precision
        if (options.has("precision")) {
            double opt_precision = options["precision"];
            precision = opt_precision;
        }
        Interface_Static::SetRVal("read.precision.val", precision);
        
        // Process additional options
        if (options.has("units")) {
            String units = options["units"];
            // Configure units handling if needed
        }
        
        if (options.has("fail_on_unknown_entity")) {
            bool fail_on_unknown = options["fail_on_unknown_entity"];
            Interface_Static::SetIVal("read.iges.faulty.entities", fail_on_unknown ? 1 : 0);
        }
        
        // Load the file
        CharString path_utf8 = file_path.utf8();
        IFSelect_ReturnStatus status = reader.ReadFile(path_utf8.get_data());
        
        if (status != IFSelect_RetDone) {
            last_error = String("Failed to read IGES file: ") + file_path;
            return Ref<ocgd_shape>();
        }
        
        // Transfer data to document
        if (!reader.Transfer(doc)) {
            last_error = "Failed to transfer IGES data to document";
            return Ref<ocgd_shape>();
        }
        
        // Get shape tool
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
        
        // Get free shapes (top-level shapes)
        TDF_LabelSequence free_shapes;
        shape_tool->GetFreeShapes(free_shapes);
        
        if (free_shapes.Length() == 0) {
            last_error = "No shapes found in IGES file";
            return Ref<ocgd_shape>();
        }
        
        TopoDS_Shape result_shape;
        
        if (free_shapes.Length() == 1) {
            // Single shape
            result_shape = shape_tool->GetShape(free_shapes.Value(1));
        } else {
            // Multiple shapes - create compound
            BRep_Builder builder;
            TopoDS_Compound compound;
            builder.MakeCompound(compound);
            
            for (int i = 1; i <= free_shapes.Length(); i++) {
                TopoDS_Shape shape = shape_tool->GetShape(free_shapes.Value(i));
                builder.Add(compound, shape);
            }
            result_shape = compound;
        }
        
        // Create and return ocgd_shape wrapper
        Ref<ocgd_shape> shape_wrapper = ocgd_shape::new_shape();
        shape_wrapper->_set_shape_internal(result_shape);
        
        return shape_wrapper;
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during IGES import: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (const std::exception& e) {
        last_error = String("Standard exception during IGES import: ") + e.what();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (...) {
        last_error = "Unknown exception during IGES import";
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    }
}

Dictionary ocgd_iges_reader::get_file_info() const {
    Dictionary info;
    info["type"] = "IGES Reader";
    info["last_error"] = last_error;
    info["transfer_colors"] = transfer_colors;
    info["transfer_names"] = transfer_names;
    info["transfer_layers"] = transfer_layers;
    info["precision"] = precision;
    return info;
}

Array ocgd_iges_reader::get_all_shapes() {
    Array shapes;
    // This would require storing the document and extracting all shapes
    // For now, return empty array - would need more complex implementation
    return shapes;
}

void ocgd_iges_reader::set_transfer_colors(bool enable) {
    transfer_colors = enable;
}

bool ocgd_iges_reader::get_transfer_colors() const {
    return transfer_colors;
}

void ocgd_iges_reader::set_transfer_names(bool enable) {
    transfer_names = enable;
}

bool ocgd_iges_reader::get_transfer_names() const {
    return transfer_names;
}

void ocgd_iges_reader::set_transfer_layers(bool enable) {
    transfer_layers = enable;
}

bool ocgd_iges_reader::get_transfer_layers() const {
    return transfer_layers;
}

void ocgd_iges_reader::set_precision(double prec) {
    precision = prec;
}

double ocgd_iges_reader::get_precision() const {
    return precision;
}

String ocgd_iges_reader::get_last_error() const {
    return last_error;
}

bool ocgd_iges_reader::has_error() const {
    return !last_error.is_empty();
}

void ocgd_iges_reader::clear_error() {
    last_error = "";
}

bool ocgd_iges_reader::validate_file(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    
    // Basic validation - check if file exists and has correct extension
    CharString path_utf8 = file_path.utf8();
    
    // Check extension
    String ext = file_path.get_extension().to_lower();
    if (ext != "iges" && ext != "igs") {
        return false;
    }
    
    // Try to open file for reading
    try {
        IGESCAFControl_Reader reader;
        IFSelect_ReturnStatus status = reader.ReadFile(path_utf8.get_data());
        return status == IFSelect_RetDone;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error validating IGES file: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        return false;
    }
}

Array ocgd_iges_reader::get_supported_extensions() const {
    Array extensions;
    extensions.append("iges");
    extensions.append("igs");
    return extensions;
}

int ocgd_iges_reader::get_entity_count() const {
    // This would require storing the reader state
    // For now, return 0 - would need more complex implementation
    return 0;
}

Dictionary ocgd_iges_reader::get_file_statistics() const {
    Dictionary stats;
    stats["entity_count"] = get_entity_count();
    stats["precision"] = precision;
    stats["has_colors"] = transfer_colors;
    stats["has_names"] = transfer_names;
    stats["has_layers"] = transfer_layers;
    return stats;
}

String ocgd_iges_reader::get_type() const {
    return "ocgd_iges_reader";
}

bool ocgd_iges_reader::is_null() const {
    return false;
}