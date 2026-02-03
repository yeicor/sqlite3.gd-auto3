#include "ocgd_step_reader.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>

// OpenCASCADE includes
#include <STEPCAFControl_Reader.hxx>
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
#include <Standard_Failure.hxx>

using namespace godot;

ocgd_step_reader::ocgd_step_reader() {
    transfer_colors = true;
    transfer_names = true;
    transfer_layers = true;
    last_error = "";
}

ocgd_step_reader::~ocgd_step_reader() {
}

void ocgd_step_reader::_bind_methods() {
    ClassDB::bind_static_method("ocgd_step_reader", D_METHOD("new_reader"), &ocgd_step_reader::new_reader);
    
    ClassDB::bind_method(D_METHOD("load_file", "file_path"), &ocgd_step_reader::load_file);
    ClassDB::bind_method(D_METHOD("load_file_with_options", "file_path", "options"), &ocgd_step_reader::load_file_with_options);
    ClassDB::bind_method(D_METHOD("get_file_info"), &ocgd_step_reader::get_file_info);
    ClassDB::bind_method(D_METHOD("get_all_shapes"), &ocgd_step_reader::get_all_shapes);
    
    ClassDB::bind_method(D_METHOD("set_transfer_colors", "enable"), &ocgd_step_reader::set_transfer_colors);
    ClassDB::bind_method(D_METHOD("get_transfer_colors"), &ocgd_step_reader::get_transfer_colors);
    ClassDB::bind_method(D_METHOD("set_transfer_names", "enable"), &ocgd_step_reader::set_transfer_names);
    ClassDB::bind_method(D_METHOD("get_transfer_names"), &ocgd_step_reader::get_transfer_names);
    ClassDB::bind_method(D_METHOD("set_transfer_layers", "enable"), &ocgd_step_reader::set_transfer_layers);
    ClassDB::bind_method(D_METHOD("get_transfer_layers"), &ocgd_step_reader::get_transfer_layers);
    
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_step_reader::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_step_reader::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_step_reader::clear_error);
    
    ClassDB::bind_method(D_METHOD("validate_file", "file_path"), &ocgd_step_reader::validate_file);
    ClassDB::bind_method(D_METHOD("get_supported_extensions"), &ocgd_step_reader::get_supported_extensions);
    
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_step_reader::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_step_reader::is_null);
}

Ref<ocgd_step_reader> ocgd_step_reader::new_reader() {
    return memnew(ocgd_step_reader);
}

Ref<ocgd_shape> ocgd_step_reader::load_file(const String& file_path) {
    Dictionary options;
    return load_file_with_options(file_path, options);
}

Ref<ocgd_shape> ocgd_step_reader::load_file_with_options(const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), Ref<ocgd_shape>(), "File path is empty");
    
    try {
        // Create XCAF document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Create STEP reader
        STEPCAFControl_Reader reader;
        
        // Configure reader based on options and properties
        reader.SetColorMode(transfer_colors);
        reader.SetNameMode(transfer_names);
        reader.SetLayerMode(transfer_layers);
        
        // Process additional options
        if (options.has("units")) {
            String units = options["units"];
            // Configure units if needed
        }
        
        if (options.has("precision")) {
            double precision = options["precision"];
            // Configure precision if needed
        }
        
        // Load the file
        CharString path_utf8 = file_path.utf8();
        IFSelect_ReturnStatus status = reader.ReadFile(path_utf8.get_data());
        
        if (status != IFSelect_RetDone) {
            last_error = String("Failed to read STEP file: ") + file_path;
            return Ref<ocgd_shape>();
        }
        
        // Transfer data to document
        if (!reader.Transfer(doc)) {
            last_error = "Failed to transfer STEP data to document";
            return Ref<ocgd_shape>();
        }
        
        // Get shape tool
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
        
        // Get free shapes (top-level shapes)
        TDF_LabelSequence free_shapes;
        shape_tool->GetFreeShapes(free_shapes);
        
        if (free_shapes.Length() == 0) {
            last_error = "No shapes found in STEP file";
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
        shape_wrapper->set_shape(result_shape);
        
        return shape_wrapper;
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during STEP import: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (const std::exception& e) {
        last_error = String("Standard exception during STEP import: ") + e.what();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (...) {
        last_error = "Unknown exception during STEP import";
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    }
}

Dictionary ocgd_step_reader::get_file_info() const {
    Dictionary info;
    info["type"] = "STEP Reader";
    info["last_error"] = last_error;
    info["transfer_colors"] = transfer_colors;
    info["transfer_names"] = transfer_names;
    info["transfer_layers"] = transfer_layers;
    return info;
}

Array ocgd_step_reader::get_all_shapes() {
    Array shapes;
    // This would require storing the document and extracting all shapes
    // For now, return empty array - would need more complex implementation
    return shapes;
}

void ocgd_step_reader::set_transfer_colors(bool enable) {
    transfer_colors = enable;
}

bool ocgd_step_reader::get_transfer_colors() const {
    return transfer_colors;
}

void ocgd_step_reader::set_transfer_names(bool enable) {
    transfer_names = enable;
}

bool ocgd_step_reader::get_transfer_names() const {
    return transfer_names;
}

void ocgd_step_reader::set_transfer_layers(bool enable) {
    transfer_layers = enable;
}

bool ocgd_step_reader::get_transfer_layers() const {
    return transfer_layers;
}

String ocgd_step_reader::get_last_error() const {
    return last_error;
}

bool ocgd_step_reader::has_error() const {
    return !last_error.is_empty();
}

void ocgd_step_reader::clear_error() {
    last_error = "";
}

bool ocgd_step_reader::validate_file(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    
    // Basic validation - check if file exists and has correct extension
    CharString path_utf8 = file_path.utf8();
    
    // Check extension
    String ext = file_path.get_extension().to_lower();
    if (ext != "step" && ext != "stp") {
        return false;
    }
    
    // Try to open file for reading
    try {
        STEPCAFControl_Reader reader;
        IFSelect_ReturnStatus status = reader.ReadFile(path_utf8.get_data());
        return status == IFSelect_RetDone;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error validating STEP file: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        return false;
    }
}

Array ocgd_step_reader::get_supported_extensions() const {
    Array extensions;
    extensions.append("step");
    extensions.append("stp");
    return extensions;
}

String ocgd_step_reader::get_type() const {
    return "ocgd_step_reader";
}

bool ocgd_step_reader::is_null() const {
    return false;
}