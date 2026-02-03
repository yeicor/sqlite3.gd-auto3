#include "ocgd_step_writer.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// OpenCASCADE includes
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Writer.hxx>
#include <STEPControl_StepModelType.hxx>
#include <Interface_Static.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_MaterialTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_Name.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <Standard_Failure.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <XCAFApp_Application.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>

using namespace godot;

void ocgd_step_writer::_bind_methods() {
    // Factory method
    ClassDB::bind_static_method("ocgd_step_writer", D_METHOD("new_step_writer"), &ocgd_step_writer::new_step_writer);

    // Core writing functionality
    ClassDB::bind_method(D_METHOD("write_shape", "shape", "file_path"), &ocgd_step_writer::write_shape);
    ClassDB::bind_method(D_METHOD("write_shapes", "shapes", "file_path"), &ocgd_step_writer::write_shapes);

    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_application_protocol", "protocol"), &ocgd_step_writer::set_application_protocol);
    ClassDB::bind_method(D_METHOD("get_application_protocol"), &ocgd_step_writer::get_application_protocol);

    ClassDB::bind_method(D_METHOD("set_schema_identifier", "schema"), &ocgd_step_writer::set_schema_identifier);
    ClassDB::bind_method(D_METHOD("get_schema_identifier"), &ocgd_step_writer::get_schema_identifier);

    ClassDB::bind_method(D_METHOD("set_originating_system", "system"), &ocgd_step_writer::set_originating_system);
    ClassDB::bind_method(D_METHOD("get_originating_system"), &ocgd_step_writer::get_originating_system);

    ClassDB::bind_method(D_METHOD("set_authorization", "auth"), &ocgd_step_writer::set_authorization);
    ClassDB::bind_method(D_METHOD("get_authorization"), &ocgd_step_writer::get_authorization);

    ClassDB::bind_method(D_METHOD("set_description", "desc"), &ocgd_step_writer::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &ocgd_step_writer::get_description);

    ClassDB::bind_method(D_METHOD("set_implementation_level", "level"), &ocgd_step_writer::set_implementation_level);
    ClassDB::bind_method(D_METHOD("get_implementation_level"), &ocgd_step_writer::get_implementation_level);

    // Unit settings
    ClassDB::bind_method(D_METHOD("set_length_unit", "unit"), &ocgd_step_writer::set_length_unit);
    ClassDB::bind_method(D_METHOD("get_length_unit"), &ocgd_step_writer::get_length_unit);

    ClassDB::bind_method(D_METHOD("set_angle_unit", "unit"), &ocgd_step_writer::set_angle_unit);
    ClassDB::bind_method(D_METHOD("get_angle_unit"), &ocgd_step_writer::get_angle_unit);

    ClassDB::bind_method(D_METHOD("set_solid_angle_unit", "unit"), &ocgd_step_writer::set_solid_angle_unit);
    ClassDB::bind_method(D_METHOD("get_solid_angle_unit"), &ocgd_step_writer::get_solid_angle_unit);

    // Feature control
    ClassDB::bind_method(D_METHOD("set_write_colors", "enable"), &ocgd_step_writer::set_write_colors);
    ClassDB::bind_method(D_METHOD("get_write_colors"), &ocgd_step_writer::get_write_colors);

    ClassDB::bind_method(D_METHOD("set_write_materials", "enable"), &ocgd_step_writer::set_write_materials);
    ClassDB::bind_method(D_METHOD("get_write_materials"), &ocgd_step_writer::get_write_materials);

    ClassDB::bind_method(D_METHOD("set_write_layers", "enable"), &ocgd_step_writer::set_write_layers);
    ClassDB::bind_method(D_METHOD("get_write_layers"), &ocgd_step_writer::get_write_layers);

    ClassDB::bind_method(D_METHOD("set_write_names", "enable"), &ocgd_step_writer::set_write_names);
    ClassDB::bind_method(D_METHOD("get_write_names"), &ocgd_step_writer::get_write_names);

    ClassDB::bind_method(D_METHOD("set_validate_on_write", "enable"), &ocgd_step_writer::set_validate_on_write);
    ClassDB::bind_method(D_METHOD("get_validate_on_write"), &ocgd_step_writer::get_validate_on_write);

    // Precision control
    ClassDB::bind_method(D_METHOD("set_write_precision_mode", "enable"), &ocgd_step_writer::set_write_precision_mode);
    ClassDB::bind_method(D_METHOD("get_write_precision_mode"), &ocgd_step_writer::get_write_precision_mode);

    ClassDB::bind_method(D_METHOD("set_precision_value", "precision"), &ocgd_step_writer::set_precision_value);
    ClassDB::bind_method(D_METHOD("get_precision_value"), &ocgd_step_writer::get_precision_value);

    // Surface options
    ClassDB::bind_method(D_METHOD("set_merge_surfaces", "enable"), &ocgd_step_writer::set_merge_surfaces);
    ClassDB::bind_method(D_METHOD("get_merge_surfaces"), &ocgd_step_writer::get_merge_surfaces);

    // Tessellation options
    ClassDB::bind_method(D_METHOD("set_write_tessellation", "enable"), &ocgd_step_writer::set_write_tessellation);
    ClassDB::bind_method(D_METHOD("get_write_tessellation"), &ocgd_step_writer::get_write_tessellation);

    ClassDB::bind_method(D_METHOD("set_tessellation_deviation", "deviation"), &ocgd_step_writer::set_tessellation_deviation);
    ClassDB::bind_method(D_METHOD("get_tessellation_deviation"), &ocgd_step_writer::get_tessellation_deviation);

    ClassDB::bind_method(D_METHOD("set_tessellation_angular_deflection", "deflection"), &ocgd_step_writer::set_tessellation_angular_deflection);
    ClassDB::bind_method(D_METHOD("get_tessellation_angular_deflection"), &ocgd_step_writer::get_tessellation_angular_deflection);

    // Advanced writing methods
    ClassDB::bind_method(D_METHOD("write_assembly", "assembly_structure", "file_path"), &ocgd_step_writer::write_assembly);
    ClassDB::bind_method(D_METHOD("write_with_metadata", "shape", "file_path", "metadata"), &ocgd_step_writer::write_with_metadata);
    ClassDB::bind_method(D_METHOD("write_shapes_with_colors", "shapes", "colors", "file_path"), &ocgd_step_writer::write_shapes_with_colors);
    ClassDB::bind_method(D_METHOD("write_shapes_with_materials", "shapes", "materials", "file_path"), &ocgd_step_writer::write_shapes_with_materials);
    ClassDB::bind_method(D_METHOD("write_shapes_with_layers", "shapes", "layer_names", "file_path"), &ocgd_step_writer::write_shapes_with_layers);

    // Validation and information
    ClassDB::bind_method(D_METHOD("validate_shape_for_step", "shape"), &ocgd_step_writer::validate_shape_for_step);
    ClassDB::bind_method(D_METHOD("get_write_statistics"), &ocgd_step_writer::get_write_statistics);
    ClassDB::bind_method(D_METHOD("get_supported_units"), &ocgd_step_writer::get_supported_units);
    ClassDB::bind_method(D_METHOD("get_supported_protocols"), &ocgd_step_writer::get_supported_protocols);
    ClassDB::bind_method(D_METHOD("get_supported_schemas"), &ocgd_step_writer::get_supported_schemas);

    // Error handling
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_step_writer::get_last_error);
    ClassDB::bind_method(D_METHOD("clear_last_error"), &ocgd_step_writer::clear_last_error);

    // Configuration presets
    ClassDB::bind_method(D_METHOD("set_configuration_preset", "preset_name"), &ocgd_step_writer::set_configuration_preset);
    ClassDB::bind_method(D_METHOD("get_current_configuration"), &ocgd_step_writer::get_current_configuration);
    ClassDB::bind_method(D_METHOD("load_configuration", "config"), &ocgd_step_writer::load_configuration);

    // File information
    ClassDB::bind_method(D_METHOD("get_step_file_info", "file_path"), &ocgd_step_writer::get_step_file_info);
    ClassDB::bind_method(D_METHOD("is_valid_step_file", "file_path"), &ocgd_step_writer::is_valid_step_file);

    // Quality control
    ClassDB::bind_method(D_METHOD("set_quality_level", "level"), &ocgd_step_writer::set_quality_level);
    ClassDB::bind_method(D_METHOD("get_quality_level"), &ocgd_step_writer::get_quality_level);

    ClassDB::bind_method(D_METHOD("set_angular_tolerance", "tolerance"), &ocgd_step_writer::set_angular_tolerance);
    ClassDB::bind_method(D_METHOD("get_angular_tolerance"), &ocgd_step_writer::get_angular_tolerance);

    ClassDB::bind_method(D_METHOD("set_linear_tolerance", "tolerance"), &ocgd_step_writer::set_linear_tolerance);
    ClassDB::bind_method(D_METHOD("get_linear_tolerance"), &ocgd_step_writer::get_linear_tolerance);

    // Advanced features
    ClassDB::bind_method(D_METHOD("set_compress_output", "enable"), &ocgd_step_writer::set_compress_output);
    ClassDB::bind_method(D_METHOD("get_compress_output"), &ocgd_step_writer::get_compress_output);

    ClassDB::bind_method(D_METHOD("set_write_shape_names", "enable"), &ocgd_step_writer::set_write_shape_names);
    ClassDB::bind_method(D_METHOD("get_write_shape_names"), &ocgd_step_writer::get_write_shape_names);

    ClassDB::bind_method(D_METHOD("set_write_surface_colors", "enable"), &ocgd_step_writer::set_write_surface_colors);
    ClassDB::bind_method(D_METHOD("get_write_surface_colors"), &ocgd_step_writer::get_write_surface_colors);

    ClassDB::bind_method(D_METHOD("set_write_curve_colors", "enable"), &ocgd_step_writer::set_write_curve_colors);
    ClassDB::bind_method(D_METHOD("get_write_curve_colors"), &ocgd_step_writer::get_write_curve_colors);
}

ocgd_step_writer::ocgd_step_writer() {
    last_error = "";
    application_protocol = "AP214";
    schema_identifier = "AUTOMOTIVE_DESIGN";
    originating_system = "OpenCASCADE.gd";
    authorization = "";
    description = "";
    implementation_level = "2;1";
    length_unit = 1.0; // mm
    angle_unit = 0.01745329252; // radians
    solid_angle_unit = 1.0; // steradians
    write_colors = true;
    write_materials = true;
    write_layers = true;
    write_names = true;
    validate_on_write = true;
    write_precision_mode = true;
    precision_value = 0.01;
    merge_surfaces = false;
    write_tessellation = false;
    tessellation_deviation = 0.1;
    tessellation_angular_deflection = 0.1;
}

ocgd_step_writer::~ocgd_step_writer() {
}

Ref<ocgd_step_writer> ocgd_step_writer::new_step_writer() {
    Ref<ocgd_step_writer> writer;
    writer.instantiate();
    return writer;
}

bool ocgd_step_writer::write_shape(const Ref<ocgd_shape>& shape, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape is null or invalid");

        // Configure Interface_Static parameters
        Interface_Static::SetCVal("write.step.schema", schema_identifier.utf8().get_data());
        Interface_Static::SetCVal("write.step.product.name", originating_system.utf8().get_data());
        Interface_Static::SetCVal("write.step.assembly.mode", write_colors ? "1" : "0");
        Interface_Static::SetRVal("write.precision.val", precision_value);
        Interface_Static::SetIVal("write.precision.mode", write_precision_mode ? 1 : 0);

        if (write_colors || write_materials || write_layers || write_names) {
            // Use STEPCAFControl_Writer for extended features
            STEPCAFControl_Writer caf_writer;
            
            // Create a document
            Handle(TDocStd_Document) doc;
            Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
            app->NewDocument("MDTV-XCAF", doc);
            
            // Get the main label and tools
            TDF_Label main_label = doc->Main();
            Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(main_label);
            Handle(XCAFDoc_ColorTool) color_tool = XCAFDoc_DocumentTool::ColorTool(main_label);
            
            // Add shape to document
            TDF_Label shape_label = shape_tool->NewShape();
            shape_tool->SetShape(shape_label, oc_shape);
            
            // Shape names would need to be managed separately since ocgd_shape doesn't have get_name()
            // if (write_names && !shape->get_name().is_empty()) {
            //     TDataStd_Name::Set(shape_label, TCollection_ExtendedString(shape->get_name().utf8().get_data()));
            // }
            
            // Transfer document to writer
            if (!caf_writer.Transfer(doc, STEPControl_AsIs)) {
                last_error = "Failed to transfer document to STEP writer";
                return false;
            }
            
            // Write to file
            IFSelect_ReturnStatus status = caf_writer.Write(file_path.utf8().get_data());
            if (status != IFSelect_RetDone) {
                last_error = "Failed to write STEP file: " + String::num_int64(static_cast<int>(status));
                return false;
            }
        } else {
            // Use simple STEPControl_Writer
            STEPControl_Writer writer;
            
            // Transfer shape
            IFSelect_ReturnStatus status = writer.Transfer(oc_shape, STEPControl_AsIs);
            if (status != IFSelect_RetDone) {
                last_error = "Failed to transfer shape to STEP writer: " + String::num_int64(static_cast<int>(status));
                return false;
            }
            
            // Write to file
            status = writer.Write(file_path.utf8().get_data());
            if (status != IFSelect_RetDone) {
                last_error = "Failed to write STEP file: " + String::num_int64(static_cast<int>(status));
                return false;
            }
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during STEP writing";
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    }
}

bool ocgd_step_writer::write_shapes(const Array& shapes, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        // Create compound shape
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape at index " + String::num_int64(i) + " is null");
            
            TopoDS_Shape oc_shape = shape->get_shape();
            ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape at index " + String::num_int64(i) + " is null or invalid");
            
            builder.Add(compound, oc_shape);
        }

        // Create temporary shape wrapper for the compound
        Ref<ocgd_shape> compound_shape = memnew(ocgd_shape);
        compound_shape->set_shape(compound);
        
        return write_shape(compound_shape, file_path);

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during STEP writing";
        ERR_PRINT("STEP write error: " + last_error);
        return false;
    }
}

void ocgd_step_writer::set_application_protocol(const String& protocol) {
    application_protocol = protocol;
}

String ocgd_step_writer::get_application_protocol() const {
    return application_protocol;
}

void ocgd_step_writer::set_schema_identifier(const String& schema) {
    schema_identifier = schema;
}

String ocgd_step_writer::get_schema_identifier() const {
    return schema_identifier;
}

void ocgd_step_writer::set_originating_system(const String& system) {
    originating_system = system;
}

String ocgd_step_writer::get_originating_system() const {
    return originating_system;
}

void ocgd_step_writer::set_authorization(const String& auth) {
    authorization = auth;
}

String ocgd_step_writer::get_authorization() const {
    return authorization;
}

void ocgd_step_writer::set_description(const String& desc) {
    description = desc;
}

String ocgd_step_writer::get_description() const {
    return description;
}

void ocgd_step_writer::set_implementation_level(const String& level) {
    implementation_level = level;
}

String ocgd_step_writer::get_implementation_level() const {
    return implementation_level;
}

void ocgd_step_writer::set_length_unit(double unit) {
    length_unit = unit;
}

double ocgd_step_writer::get_length_unit() const {
    return length_unit;
}

void ocgd_step_writer::set_angle_unit(double unit) {
    angle_unit = unit;
}

double ocgd_step_writer::get_angle_unit() const {
    return angle_unit;
}

void ocgd_step_writer::set_solid_angle_unit(double unit) {
    solid_angle_unit = unit;
}

double ocgd_step_writer::get_solid_angle_unit() const {
    return solid_angle_unit;
}

void ocgd_step_writer::set_write_colors(bool enable) {
    write_colors = enable;
}

bool ocgd_step_writer::get_write_colors() const {
    return write_colors;
}

void ocgd_step_writer::set_write_materials(bool enable) {
    write_materials = enable;
}

bool ocgd_step_writer::get_write_materials() const {
    return write_materials;
}

void ocgd_step_writer::set_write_layers(bool enable) {
    write_layers = enable;
}

bool ocgd_step_writer::get_write_layers() const {
    return write_layers;
}

void ocgd_step_writer::set_write_names(bool enable) {
    write_names = enable;
}

bool ocgd_step_writer::get_write_names() const {
    return write_names;
}

void ocgd_step_writer::set_validate_on_write(bool enable) {
    validate_on_write = enable;
}

bool ocgd_step_writer::get_validate_on_write() const {
    return validate_on_write;
}

void ocgd_step_writer::set_write_precision_mode(bool enable) {
    write_precision_mode = enable;
}

bool ocgd_step_writer::get_write_precision_mode() const {
    return write_precision_mode;
}

void ocgd_step_writer::set_precision_value(double precision) {
    precision_value = precision;
}

double ocgd_step_writer::get_precision_value() const {
    return precision_value;
}

void ocgd_step_writer::set_merge_surfaces(bool enable) {
    merge_surfaces = enable;
}

bool ocgd_step_writer::get_merge_surfaces() const {
    return merge_surfaces;
}

void ocgd_step_writer::set_write_tessellation(bool enable) {
    write_tessellation = enable;
}

bool ocgd_step_writer::get_write_tessellation() const {
    return write_tessellation;
}

void ocgd_step_writer::set_tessellation_deviation(double deviation) {
    tessellation_deviation = deviation;
}

double ocgd_step_writer::get_tessellation_deviation() const {
    return tessellation_deviation;
}

void ocgd_step_writer::set_tessellation_angular_deflection(double deflection) {
    tessellation_angular_deflection = deflection;
}

double ocgd_step_writer::get_tessellation_angular_deflection() const {
    return tessellation_angular_deflection;
}

bool ocgd_step_writer::write_assembly(const Dictionary& assembly_structure, const String& file_path) {
    ERR_FAIL_COND_V_MSG(assembly_structure.is_empty(), false, "Assembly structure is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        STEPCAFControl_Writer caf_writer;
        
        // Create a document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get the main label and tools
        TDF_Label main_label = doc->Main();
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(main_label);
        
        // Process assembly structure recursively
        // This is a simplified implementation - real assemblies would need position/orientation handling
        Array shapes = assembly_structure.get("shapes", Array());
        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            if (shape.is_valid()) {
                TDF_Label shape_label = shape_tool->NewShape();
                shape_tool->SetShape(shape_label, shape->get_shape());
                
                // Shape names would need to be managed separately since ocgd_shape doesn't have get_name()
                // if (!shape->get_name().is_empty()) {
                //     TDataStd_Name::Set(shape_label, TCollection_ExtendedString(shape->get_name().utf8().get_data()));
                // }
            }
        }
        
        // Transfer document to writer
        if (!caf_writer.Transfer(doc, STEPControl_AsIs)) {
            last_error = "Failed to transfer assembly document to STEP writer";
            return false;
        }
        
        // Write to file
        IFSelect_ReturnStatus status = caf_writer.Write(file_path.utf8().get_data());
        if (status != IFSelect_RetDone) {
            last_error = "Failed to write STEP assembly file: " + String::num_int64(static_cast<int>(status));
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("STEP assembly write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("STEP assembly write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during STEP assembly writing";
        ERR_PRINT("STEP assembly write error: " + last_error);
        return false;
    }
}

bool ocgd_step_writer::write_with_metadata(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& metadata) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    // Apply metadata to writer configuration
    if (metadata.has("description")) {
        set_description(metadata["description"]);
    }
    if (metadata.has("authorization")) {
        set_authorization(metadata["authorization"]);
    }
    if (metadata.has("originating_system")) {
        set_originating_system(metadata["originating_system"]);
    }

    return write_shape(shape, file_path);
}

bool ocgd_step_writer::write_shapes_with_colors(const Array& shapes, const Array& colors, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.size() != colors.size(), false, "Shapes and colors arrays must have the same size");
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        STEPCAFControl_Writer caf_writer;
        
        // Create a document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get the main label and tools
        TDF_Label main_label = doc->Main();
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(main_label);
        Handle(XCAFDoc_ColorTool) color_tool = XCAFDoc_DocumentTool::ColorTool(main_label);
        
        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape at index " + String::num_int64(i) + " is null");
            
            TopoDS_Shape oc_shape = shape->get_shape();
            ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape at index " + String::num_int64(i) + " is null or invalid");
            
            // Add shape to document
            TDF_Label shape_label = shape_tool->NewShape();
            shape_tool->SetShape(shape_label, oc_shape);
            
            // Add color if provided
            Vector3 color = colors[i];
            Quantity_Color oc_color(color.x, color.y, color.z, Quantity_TOC_RGB);
            color_tool->SetColor(shape_label, oc_color, XCAFDoc_ColorSurf);
        }
        
        // Transfer document to writer
        if (!caf_writer.Transfer(doc, STEPControl_AsIs)) {
            last_error = "Failed to transfer colored shapes document to STEP writer";
            return false;
        }
        
        // Write to file
        IFSelect_ReturnStatus status = caf_writer.Write(file_path.utf8().get_data());
        if (status != IFSelect_RetDone) {
            last_error = "Failed to write STEP file with colors: " + String::num_int64(static_cast<int>(status));
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("STEP colored write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("STEP colored write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during STEP colored writing";
        ERR_PRINT("STEP colored write error: " + last_error);
        return false;
    }
}

bool ocgd_step_writer::write_shapes_with_materials(const Array& shapes, const Array& materials, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.size() != materials.size(), false, "Shapes and materials arrays must have the same size");
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    // For now, treat materials as colors (simplified implementation)
    Array colors;
    for (int i = 0; i < materials.size(); i++) {
        Dictionary material = materials[i];
        Vector3 color = material.get("color", Vector3(0.5, 0.5, 0.5));
        colors.append(color);
    }

    return write_shapes_with_colors(shapes, colors, file_path);
}

bool ocgd_step_writer::write_shapes_with_layers(const Array& shapes, const Array& layer_names, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.size() != layer_names.size(), false, "Shapes and layer names arrays must have the same size");
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        STEPCAFControl_Writer caf_writer;
        
        // Create a document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get the main label and tools
        TDF_Label main_label = doc->Main();
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(main_label);
        Handle(XCAFDoc_LayerTool) layer_tool = XCAFDoc_DocumentTool::LayerTool(main_label);
        
        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape at index " + String::num_int64(i) + " is null");
            
            TopoDS_Shape oc_shape = shape->get_shape();
            ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape at index " + String::num_int64(i) + " is null or invalid");
            
            // Add shape to document
            TDF_Label shape_label = shape_tool->NewShape();
            shape_tool->SetShape(shape_label, oc_shape);
            
            // Add to layer
            String layer_name = layer_names[i];
            TDF_Label layer_label = layer_tool->AddLayer(TCollection_ExtendedString(layer_name.utf8().get_data()));
            layer_tool->SetLayer(shape_label, layer_label);
        }
        
        // Transfer document to writer
        if (!caf_writer.Transfer(doc, STEPControl_AsIs)) {
            last_error = "Failed to transfer layered shapes document to STEP writer";
            return false;
        }
        
        // Write to file
        IFSelect_ReturnStatus status = caf_writer.Write(file_path.utf8().get_data());
        if (status != IFSelect_RetDone) {
            last_error = "Failed to write STEP file with layers: " + String::num_int64(static_cast<int>(status));
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("STEP layered write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("STEP layered write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during STEP layered writing";
        ERR_PRINT("STEP layered write error: " + last_error);
        return false;
    }
}

bool ocgd_step_writer::validate_shape_for_step(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape is null or invalid");

        // Basic validation
        BRepCheck_Analyzer analyzer(oc_shape);
        if (!analyzer.IsValid()) {
            last_error = "Shape is not valid for STEP export";
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error during validation: " + String(e.GetMessageString());
        return false;
    } catch (...) {
        last_error = "Unknown error during shape validation";
        return false;
    }
}

Dictionary ocgd_step_writer::get_write_statistics() {
    Dictionary stats;
    stats["last_error"] = last_error;
    stats["application_protocol"] = application_protocol;
    stats["schema_identifier"] = schema_identifier;
    stats["write_colors"] = write_colors;
    stats["write_materials"] = write_materials;
    stats["write_layers"] = write_layers;
    stats["precision_value"] = precision_value;
    return stats;
}

Array ocgd_step_writer::get_supported_units() {
    Array units;
    units.append("mm"); // millimeters
    units.append("m");  // meters
    units.append("in"); // inches
    units.append("ft"); // feet
    return units;
}

Array ocgd_step_writer::get_supported_protocols() {
    Array protocols;
    protocols.append("AP203");
    protocols.append("AP214");
    protocols.append("AP242");
    return protocols;
}

Array ocgd_step_writer::get_supported_schemas() {
    Array schemas;
    schemas.append("CONFIG_CONTROL_DESIGN");
    schemas.append("AUTOMOTIVE_DESIGN");
    schemas.append("STRUCTURAL_ANALYSIS_DESIGN");
    return schemas;
}

String ocgd_step_writer::get_last_error() const {
    return last_error;
}

void ocgd_step_writer::clear_last_error() {
    last_error = "";
}

void ocgd_step_writer::set_configuration_preset(const String& preset_name) {
    if (preset_name == "CAD_EXPORT") {
        set_write_colors(true);
        set_write_materials(true);
        set_write_layers(true);
        set_write_names(true);
        set_precision_value(0.01);
        set_application_protocol("AP214");
        set_schema_identifier("AUTOMOTIVE_DESIGN");
    } else if (preset_name == "MINIMAL") {
        set_write_colors(false);
        set_write_materials(false);
        set_write_layers(false);
        set_write_names(false);
        set_precision_value(0.1);
        set_application_protocol("AP203");
        set_schema_identifier("CONFIG_CONTROL_DESIGN");
    } else if (preset_name == "HIGH_PRECISION") {
        set_write_colors(true);
        set_write_materials(true);
        set_write_layers(true);
        set_write_names(true);
        set_precision_value(0.001);
        set_application_protocol("AP242");
        set_schema_identifier("AUTOMOTIVE_DESIGN");
    }
}

Dictionary ocgd_step_writer::get_current_configuration() {
    Dictionary config;
    config["application_protocol"] = application_protocol;
    config["schema_identifier"] = schema_identifier;
    config["originating_system"] = originating_system;
    config["authorization"] = authorization;
    config["description"] = description;
    config["implementation_level"] = implementation_level;
    config["length_unit"] = length_unit;
    config["angle_unit"] = angle_unit;
    config["solid_angle_unit"] = solid_angle_unit;
    config["write_colors"] = write_colors;
    config["write_materials"] = write_materials;
    config["write_layers"] = write_layers;
    config["write_names"] = write_names;
    config["validate_on_write"] = validate_on_write;
    config["write_precision_mode"] = write_precision_mode;
    config["precision_value"] = precision_value;
    config["merge_surfaces"] = merge_surfaces;
    config["write_tessellation"] = write_tessellation;
    config["tessellation_deviation"] = tessellation_deviation;
    config["tessellation_angular_deflection"] = tessellation_angular_deflection;
    return config;
}

void ocgd_step_writer::load_configuration(const Dictionary& config) {
    if (config.has("application_protocol")) set_application_protocol(config["application_protocol"]);
    if (config.has("schema_identifier")) set_schema_identifier(config["schema_identifier"]);
    if (config.has("originating_system")) set_originating_system(config["originating_system"]);
    if (config.has("authorization")) set_authorization(config["authorization"]);
    if (config.has("description")) set_description(config["description"]);
    if (config.has("implementation_level")) set_implementation_level(config["implementation_level"]);
    if (config.has("length_unit")) set_length_unit(config["length_unit"]);
    if (config.has("angle_unit")) set_angle_unit(config["angle_unit"]);
    if (config.has("solid_angle_unit")) set_solid_angle_unit(config["solid_angle_unit"]);
    if (config.has("write_colors")) set_write_colors(config["write_colors"]);
    if (config.has("write_materials")) set_write_materials(config["write_materials"]);
    if (config.has("write_layers")) set_write_layers(config["write_layers"]);
    if (config.has("write_names")) set_write_names(config["write_names"]);
    if (config.has("validate_on_write")) set_validate_on_write(config["validate_on_write"]);
    if (config.has("write_precision_mode")) set_write_precision_mode(config["write_precision_mode"]);
    if (config.has("precision_value")) set_precision_value(config["precision_value"]);
    if (config.has("merge_surfaces")) set_merge_surfaces(config["merge_surfaces"]);
    if (config.has("write_tessellation")) set_write_tessellation(config["write_tessellation"]);
    if (config.has("tessellation_deviation")) set_tessellation_deviation(config["tessellation_deviation"]);
    if (config.has("tessellation_angular_deflection")) set_tessellation_angular_deflection(config["tessellation_angular_deflection"]);
}

Dictionary ocgd_step_writer::get_step_file_info(const String& file_path) {
    Dictionary info;
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), info, "File path is empty");

    try {
        // Check if file exists and is readable
        std::ifstream file(file_path.utf8().get_data());
        if (!file.is_open()) {
            info["error"] = "File does not exist or is not readable";
            return info;
        }

        // Read first few lines to extract header information
        std::string line;
        int line_count = 0;
        while (std::getline(file, line) && line_count < 10) {
            if (line.find("FILE_DESCRIPTION") != std::string::npos) {
                info["has_description"] = true;
            }
            if (line.find("FILE_NAME") != std::string::npos) {
                info["has_name"] = true;
            }
            if (line.find("FILE_SCHEMA") != std::string::npos) {
                info["has_schema"] = true;
            }
            line_count++;
        }

        file.close();
        info["is_valid_step"] = true;
        info["line_count"] = line_count;

    } catch (...) {
        info["error"] = "Error reading STEP file";
    }

    return info;
}

bool ocgd_step_writer::is_valid_step_file(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        std::ifstream file(file_path.utf8().get_data());
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        if (std::getline(file, line)) {
            // Check for STEP file header
            if (line.find("ISO-10303") != std::string::npos || line.find("STEP") != std::string::npos) {
                file.close();
                return true;
            }
        }

        file.close();
        return false;

    } catch (...) {
        return false;
    }
}

void ocgd_step_writer::set_quality_level(int level) {
    // Quality level affects precision and tessellation settings
    switch (level) {
        case 1: // Low quality
            set_precision_value(0.1);
            set_tessellation_deviation(0.5);
            break;
        case 2: // Medium quality
            set_precision_value(0.01);
            set_tessellation_deviation(0.1);
            break;
        case 3: // High quality
            set_precision_value(0.001);
            set_tessellation_deviation(0.01);
            break;
        default:
            set_precision_value(0.01);
            set_tessellation_deviation(0.1);
            break;
    }
}

int ocgd_step_writer::get_quality_level() const {
    if (precision_value >= 0.1) return 1;
    else if (precision_value >= 0.01) return 2;
    else return 3;
}

void ocgd_step_writer::set_angular_tolerance(double tolerance) {
    angle_unit = tolerance;
}

double ocgd_step_writer::get_angular_tolerance() const {
    return angle_unit;
}

void ocgd_step_writer::set_linear_tolerance(double tolerance) {
    precision_value = tolerance;
}

double ocgd_step_writer::get_linear_tolerance() const {
    return precision_value;
}

void ocgd_step_writer::set_compress_output(bool enable) {
    // STEP format doesn't support compression natively
    // This could be used for post-processing compression
}

bool ocgd_step_writer::get_compress_output() const {
    return false; // Not supported
}

void ocgd_step_writer::set_write_shape_names(bool enable) {
    write_names = enable;
}

bool ocgd_step_writer::get_write_shape_names() const {
    return write_names;
}

void ocgd_step_writer::set_write_surface_colors(bool enable) {
    write_colors = enable;
}

bool ocgd_step_writer::get_write_surface_colors() const {
    return write_colors;
}

void ocgd_step_writer::set_write_curve_colors(bool enable) {
    // For STEP, curve colors are part of general color writing
    write_colors = enable;
}

bool ocgd_step_writer::get_write_curve_colors() const {
    return write_colors;
}