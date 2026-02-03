#include "ocgd_iges_writer.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// OpenCASCADE includes
#include <IGESControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <Interface_Static.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <Standard_Failure.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_LayerTool.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_Name.hxx>
#include <Quantity_Color.hxx>
#include <XCAFApp_Application.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>

using namespace godot;

void ocgd_iges_writer::_bind_methods() {
    // Factory method
    ClassDB::bind_static_method("ocgd_iges_writer", D_METHOD("new_iges_writer"), &ocgd_iges_writer::new_iges_writer);

    // Core writing functionality
    ClassDB::bind_method(D_METHOD("write_shape", "shape", "file_path"), &ocgd_iges_writer::write_shape);
    ClassDB::bind_method(D_METHOD("write_shapes", "shapes", "file_path"), &ocgd_iges_writer::write_shapes);

    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_system_identifier", "identifier"), &ocgd_iges_writer::set_system_identifier);
    ClassDB::bind_method(D_METHOD("get_system_identifier"), &ocgd_iges_writer::get_system_identifier);

    ClassDB::bind_method(D_METHOD("set_preprocessor_version", "version"), &ocgd_iges_writer::set_preprocessor_version);
    ClassDB::bind_method(D_METHOD("get_preprocessor_version"), &ocgd_iges_writer::get_preprocessor_version);

    ClassDB::bind_method(D_METHOD("set_author_name", "name"), &ocgd_iges_writer::set_author_name);
    ClassDB::bind_method(D_METHOD("get_author_name"), &ocgd_iges_writer::get_author_name);

    ClassDB::bind_method(D_METHOD("set_organization", "org"), &ocgd_iges_writer::set_organization);
    ClassDB::bind_method(D_METHOD("get_organization"), &ocgd_iges_writer::get_organization);

    ClassDB::bind_method(D_METHOD("set_description", "desc"), &ocgd_iges_writer::set_description);
    ClassDB::bind_method(D_METHOD("get_description"), &ocgd_iges_writer::get_description);

    ClassDB::bind_method(D_METHOD("set_model_space_units", "units"), &ocgd_iges_writer::set_model_space_units);
    ClassDB::bind_method(D_METHOD("get_model_space_units"), &ocgd_iges_writer::get_model_space_units);

    ClassDB::bind_method(D_METHOD("set_global_scale", "scale"), &ocgd_iges_writer::set_global_scale);
    ClassDB::bind_method(D_METHOD("get_global_scale"), &ocgd_iges_writer::get_global_scale);

    // Feature control
    ClassDB::bind_method(D_METHOD("set_write_colors", "enable"), &ocgd_iges_writer::set_write_colors);
    ClassDB::bind_method(D_METHOD("get_write_colors"), &ocgd_iges_writer::get_write_colors);

    ClassDB::bind_method(D_METHOD("set_write_layers", "enable"), &ocgd_iges_writer::set_write_layers);
    ClassDB::bind_method(D_METHOD("get_write_layers"), &ocgd_iges_writer::get_write_layers);

    ClassDB::bind_method(D_METHOD("set_write_names", "enable"), &ocgd_iges_writer::set_write_names);
    ClassDB::bind_method(D_METHOD("get_write_names"), &ocgd_iges_writer::get_write_names);

    ClassDB::bind_method(D_METHOD("set_validate_on_write", "enable"), &ocgd_iges_writer::set_validate_on_write);
    ClassDB::bind_method(D_METHOD("get_validate_on_write"), &ocgd_iges_writer::get_validate_on_write);

    // Precision control
    ClassDB::bind_method(D_METHOD("set_write_precision_mode", "enable"), &ocgd_iges_writer::set_write_precision_mode);
    ClassDB::bind_method(D_METHOD("get_write_precision_mode"), &ocgd_iges_writer::get_write_precision_mode);

    ClassDB::bind_method(D_METHOD("set_precision_value", "precision"), &ocgd_iges_writer::set_precision_value);
    ClassDB::bind_method(D_METHOD("get_precision_value"), &ocgd_iges_writer::get_precision_value);

    // Surface options
    ClassDB::bind_method(D_METHOD("set_merge_surfaces", "enable"), &ocgd_iges_writer::set_merge_surfaces);
    ClassDB::bind_method(D_METHOD("get_merge_surfaces"), &ocgd_iges_writer::get_merge_surfaces);

    // IGES version and standards
    ClassDB::bind_method(D_METHOD("set_iges_version", "version"), &ocgd_iges_writer::set_iges_version);
    ClassDB::bind_method(D_METHOD("get_iges_version"), &ocgd_iges_writer::get_iges_version);

    ClassDB::bind_method(D_METHOD("set_drafting_standard", "standard"), &ocgd_iges_writer::set_drafting_standard);
    ClassDB::bind_method(D_METHOD("get_drafting_standard"), &ocgd_iges_writer::get_drafting_standard);

    ClassDB::bind_method(D_METHOD("set_model_units", "units"), &ocgd_iges_writer::set_model_units);
    ClassDB::bind_method(D_METHOD("get_model_units"), &ocgd_iges_writer::get_model_units);

    ClassDB::bind_method(D_METHOD("set_unit_scale_factor", "factor"), &ocgd_iges_writer::set_unit_scale_factor);
    ClassDB::bind_method(D_METHOD("get_unit_scale_factor"), &ocgd_iges_writer::get_unit_scale_factor);

    // Entity type control
    ClassDB::bind_method(D_METHOD("set_write_brep_entities", "enable"), &ocgd_iges_writer::set_write_brep_entities);
    ClassDB::bind_method(D_METHOD("get_write_brep_entities"), &ocgd_iges_writer::get_write_brep_entities);

    ClassDB::bind_method(D_METHOD("set_write_trimmed_surfaces", "enable"), &ocgd_iges_writer::set_write_trimmed_surfaces);
    ClassDB::bind_method(D_METHOD("get_write_trimmed_surfaces"), &ocgd_iges_writer::get_write_trimmed_surfaces);

    ClassDB::bind_method(D_METHOD("set_write_analytical_surfaces", "enable"), &ocgd_iges_writer::set_write_analytical_surfaces);
    ClassDB::bind_method(D_METHOD("get_write_analytical_surfaces"), &ocgd_iges_writer::get_write_analytical_surfaces);

    ClassDB::bind_method(D_METHOD("set_optimize_curves", "enable"), &ocgd_iges_writer::set_optimize_curves);
    ClassDB::bind_method(D_METHOD("get_optimize_curves"), &ocgd_iges_writer::get_optimize_curves);

    // Tolerance settings
    ClassDB::bind_method(D_METHOD("set_curve_tolerance", "tolerance"), &ocgd_iges_writer::set_curve_tolerance);
    ClassDB::bind_method(D_METHOD("get_curve_tolerance"), &ocgd_iges_writer::get_curve_tolerance);

    ClassDB::bind_method(D_METHOD("set_surface_tolerance", "tolerance"), &ocgd_iges_writer::set_surface_tolerance);
    ClassDB::bind_method(D_METHOD("get_surface_tolerance"), &ocgd_iges_writer::get_surface_tolerance);

    // Advanced writing methods
    ClassDB::bind_method(D_METHOD("write_with_metadata", "shape", "file_path", "metadata"), &ocgd_iges_writer::write_with_metadata);
    ClassDB::bind_method(D_METHOD("write_shapes_with_colors", "shapes", "colors", "file_path"), &ocgd_iges_writer::write_shapes_with_colors);
    ClassDB::bind_method(D_METHOD("write_shapes_with_layers", "shapes", "layer_names", "file_path"), &ocgd_iges_writer::write_shapes_with_layers);

    // Validation and information
    ClassDB::bind_method(D_METHOD("validate_shape_for_iges", "shape"), &ocgd_iges_writer::validate_shape_for_iges);
    ClassDB::bind_method(D_METHOD("get_write_statistics"), &ocgd_iges_writer::get_write_statistics);
    ClassDB::bind_method(D_METHOD("get_supported_units"), &ocgd_iges_writer::get_supported_units);
    ClassDB::bind_method(D_METHOD("get_supported_versions"), &ocgd_iges_writer::get_supported_versions);

    // Error handling
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_iges_writer::get_last_error);
    ClassDB::bind_method(D_METHOD("clear_last_error"), &ocgd_iges_writer::clear_last_error);

    // Configuration presets
    ClassDB::bind_method(D_METHOD("set_configuration_preset", "preset_name"), &ocgd_iges_writer::set_configuration_preset);
    ClassDB::bind_method(D_METHOD("get_current_configuration"), &ocgd_iges_writer::get_current_configuration);
    ClassDB::bind_method(D_METHOD("load_configuration", "config"), &ocgd_iges_writer::load_configuration);

    // File information
    ClassDB::bind_method(D_METHOD("get_iges_file_info", "file_path"), &ocgd_iges_writer::get_iges_file_info);
    ClassDB::bind_method(D_METHOD("is_valid_iges_file", "file_path"), &ocgd_iges_writer::is_valid_iges_file);

    // Quality control
    ClassDB::bind_method(D_METHOD("set_quality_level", "level"), &ocgd_iges_writer::set_quality_level);
    ClassDB::bind_method(D_METHOD("get_quality_level"), &ocgd_iges_writer::get_quality_level);

    ClassDB::bind_method(D_METHOD("set_angular_tolerance", "tolerance"), &ocgd_iges_writer::set_angular_tolerance);
    ClassDB::bind_method(D_METHOD("get_angular_tolerance"), &ocgd_iges_writer::get_angular_tolerance);

    ClassDB::bind_method(D_METHOD("set_linear_tolerance", "tolerance"), &ocgd_iges_writer::set_linear_tolerance);
    ClassDB::bind_method(D_METHOD("get_linear_tolerance"), &ocgd_iges_writer::get_linear_tolerance);

    // Advanced features
    ClassDB::bind_method(D_METHOD("set_write_parameter_space_curves", "enable"), &ocgd_iges_writer::set_write_parameter_space_curves);
    ClassDB::bind_method(D_METHOD("get_write_parameter_space_curves"), &ocgd_iges_writer::get_write_parameter_space_curves);

    ClassDB::bind_method(D_METHOD("set_write_isoparametric_curves", "enable"), &ocgd_iges_writer::set_write_isoparametric_curves);
    ClassDB::bind_method(D_METHOD("get_write_isoparametric_curves"), &ocgd_iges_writer::get_write_isoparametric_curves);

    ClassDB::bind_method(D_METHOD("set_convert_surfaces_to_bsplines", "enable"), &ocgd_iges_writer::set_convert_surfaces_to_bsplines);
    ClassDB::bind_method(D_METHOD("get_convert_surfaces_to_bsplines"), &ocgd_iges_writer::get_convert_surfaces_to_bsplines);

    ClassDB::bind_method(D_METHOD("set_convert_curves_to_bsplines", "enable"), &ocgd_iges_writer::set_convert_curves_to_bsplines);
    ClassDB::bind_method(D_METHOD("get_convert_curves_to_bsplines"), &ocgd_iges_writer::get_convert_curves_to_bsplines);
}

ocgd_iges_writer::ocgd_iges_writer() {
    last_error = "";
    system_identifier = "OpenCASCADE.gd";
    preprocessor_version = "1.0";
    author_name = "";
    organization = "";
    description = "";
    model_space_units = "MM";
    global_scale = 1.0;
    write_colors = true;
    write_layers = true;
    write_names = true;
    validate_on_write = true;
    write_precision_mode = true;
    precision_value = 0.01;
    merge_surfaces = false;
    iges_version = 11; // IGES 5.3
    drafting_standard = "NONE";
    model_units = "MM";
    unit_scale_factor = 1.0;
    write_brep_entities = true;
    write_trimmed_surfaces = true;
    write_analytical_surfaces = true;
    optimize_curves = true;
    curve_tolerance = 0.01;
    surface_tolerance = 0.01;
    write_isoparametric_curves = false;
}

ocgd_iges_writer::~ocgd_iges_writer() {
}

Ref<ocgd_iges_writer> ocgd_iges_writer::new_iges_writer() {
    Ref<ocgd_iges_writer> writer;
    writer.instantiate();
    return writer;
}

bool ocgd_iges_writer::write_shape(const Ref<ocgd_shape>& shape, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape is null or invalid");

        // Configure Interface_Static parameters
        Interface_Static::SetCVal("write.iges.header.author", author_name.utf8().get_data());
        Interface_Static::SetCVal("write.iges.header.company", organization.utf8().get_data());
        Interface_Static::SetCVal("write.iges.header.product", description.utf8().get_data());
        Interface_Static::SetCVal("write.iges.unit", model_units.utf8().get_data());
        Interface_Static::SetRVal("write.precision.val", precision_value);
        Interface_Static::SetIVal("write.precision.mode", write_precision_mode ? 1 : 0);
        Interface_Static::SetIVal("write.iges.brep.mode", write_brep_entities ? 1 : 0);

        if (write_colors || write_layers || write_names) {
            // Use IGESCAFControl_Writer for extended features
            IGESCAFControl_Writer caf_writer;
            
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
            if (!caf_writer.Transfer(doc)) {
                last_error = "Failed to transfer document to IGES writer";
                return false;
            }
            
            // Write to file
            if (!caf_writer.Write(file_path.utf8().get_data())) {
                last_error = "Failed to write IGES file";
                return false;
            }
        } else {
            // Use simple IGESControl_Writer
            IGESControl_Writer writer;
            
            // Transfer shape
            if (!writer.AddShape(oc_shape)) {
                last_error = "Failed to add shape to IGES writer";
                return false;
            }
            
            // Compute model and write to file
            writer.ComputeModel();
            if (!writer.Write(file_path.utf8().get_data())) {
                last_error = "Failed to write IGES file";
                return false;
            }
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during IGES writing";
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    }
}

bool ocgd_iges_writer::write_shapes(const Array& shapes, const String& file_path) {
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
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during IGES writing";
        ERR_PRINT("IGES write error: " + last_error);
        return false;
    }
}

void ocgd_iges_writer::set_system_identifier(const String& identifier) {
    system_identifier = identifier;
}

String ocgd_iges_writer::get_system_identifier() const {
    return system_identifier;
}

void ocgd_iges_writer::set_preprocessor_version(const String& version) {
    preprocessor_version = version;
}

String ocgd_iges_writer::get_preprocessor_version() const {
    return preprocessor_version;
}

void ocgd_iges_writer::set_author_name(const String& name) {
    author_name = name;
}

String ocgd_iges_writer::get_author_name() const {
    return author_name;
}

void ocgd_iges_writer::set_organization(const String& org) {
    organization = org;
}

String ocgd_iges_writer::get_organization() const {
    return organization;
}

void ocgd_iges_writer::set_description(const String& desc) {
    description = desc;
}

String ocgd_iges_writer::get_description() const {
    return description;
}

void ocgd_iges_writer::set_model_space_units(const String& units) {
    model_space_units = units;
}

String ocgd_iges_writer::get_model_space_units() const {
    return model_space_units;
}

void ocgd_iges_writer::set_global_scale(double scale) {
    global_scale = scale;
}

double ocgd_iges_writer::get_global_scale() const {
    return global_scale;
}

void ocgd_iges_writer::set_write_colors(bool enable) {
    write_colors = enable;
}

bool ocgd_iges_writer::get_write_colors() const {
    return write_colors;
}

void ocgd_iges_writer::set_write_layers(bool enable) {
    write_layers = enable;
}

bool ocgd_iges_writer::get_write_layers() const {
    return write_layers;
}

void ocgd_iges_writer::set_write_names(bool enable) {
    write_names = enable;
}

bool ocgd_iges_writer::get_write_names() const {
    return write_names;
}

void ocgd_iges_writer::set_validate_on_write(bool enable) {
    validate_on_write = enable;
}

bool ocgd_iges_writer::get_validate_on_write() const {
    return validate_on_write;
}

void ocgd_iges_writer::set_write_precision_mode(bool enable) {
    write_precision_mode = enable;
}

bool ocgd_iges_writer::get_write_precision_mode() const {
    return write_precision_mode;
}

void ocgd_iges_writer::set_precision_value(double precision) {
    precision_value = precision;
}

double ocgd_iges_writer::get_precision_value() const {
    return precision_value;
}

void ocgd_iges_writer::set_merge_surfaces(bool enable) {
    merge_surfaces = enable;
}

bool ocgd_iges_writer::get_merge_surfaces() const {
    return merge_surfaces;
}

void ocgd_iges_writer::set_iges_version(int version) {
    iges_version = version;
}

int ocgd_iges_writer::get_iges_version() const {
    return iges_version;
}

void ocgd_iges_writer::set_drafting_standard(const String& standard) {
    drafting_standard = standard;
}

String ocgd_iges_writer::get_drafting_standard() const {
    return drafting_standard;
}

void ocgd_iges_writer::set_model_units(const String& units) {
    model_units = units;
}

String ocgd_iges_writer::get_model_units() const {
    return model_units;
}

void ocgd_iges_writer::set_unit_scale_factor(double factor) {
    unit_scale_factor = factor;
}

double ocgd_iges_writer::get_unit_scale_factor() const {
    return unit_scale_factor;
}

void ocgd_iges_writer::set_write_brep_entities(bool enable) {
    write_brep_entities = enable;
}

bool ocgd_iges_writer::get_write_brep_entities() const {
    return write_brep_entities;
}

void ocgd_iges_writer::set_write_trimmed_surfaces(bool enable) {
    write_trimmed_surfaces = enable;
}

bool ocgd_iges_writer::get_write_trimmed_surfaces() const {
    return write_trimmed_surfaces;
}

void ocgd_iges_writer::set_write_analytical_surfaces(bool enable) {
    write_analytical_surfaces = enable;
}

bool ocgd_iges_writer::get_write_analytical_surfaces() const {
    return write_analytical_surfaces;
}

void ocgd_iges_writer::set_optimize_curves(bool enable) {
    optimize_curves = enable;
}

bool ocgd_iges_writer::get_optimize_curves() const {
    return optimize_curves;
}

void ocgd_iges_writer::set_curve_tolerance(double tolerance) {
    curve_tolerance = tolerance;
}

double ocgd_iges_writer::get_curve_tolerance() const {
    return curve_tolerance;
}

void ocgd_iges_writer::set_surface_tolerance(double tolerance) {
    surface_tolerance = tolerance;
}

double ocgd_iges_writer::get_surface_tolerance() const {
    return surface_tolerance;
}

bool ocgd_iges_writer::write_with_metadata(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& metadata) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    // Apply metadata to writer configuration
    if (metadata.has("description")) {
        set_description(metadata["description"]);
    }
    if (metadata.has("author_name")) {
        set_author_name(metadata["author_name"]);
    }
    if (metadata.has("organization")) {
        set_organization(metadata["organization"]);
    }

    return write_shape(shape, file_path);
}

bool ocgd_iges_writer::write_shapes_with_colors(const Array& shapes, const Array& colors, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.size() != colors.size(), false, "Shapes and colors arrays must have the same size");
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        IGESCAFControl_Writer caf_writer;
        
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
        if (!caf_writer.Transfer(doc)) {
            last_error = "Failed to transfer colored shapes document to IGES writer";
            return false;
        }
        
        // Write to file
        if (!caf_writer.Write(file_path.utf8().get_data())) {
            last_error = "Failed to write IGES file with colors";
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("IGES colored write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("IGES colored write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during IGES colored writing";
        ERR_PRINT("IGES colored write error: " + last_error);
        return false;
    }
}

bool ocgd_iges_writer::write_shapes_with_layers(const Array& shapes, const Array& layer_names, const String& file_path) {
    ERR_FAIL_COND_V_MSG(shapes.size() != layer_names.size(), false, "Shapes and layer names arrays must have the same size");
    ERR_FAIL_COND_V_MSG(shapes.is_empty(), false, "Shapes array is empty");
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        IGESCAFControl_Writer caf_writer;
        
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
        if (!caf_writer.Transfer(doc)) {
            last_error = "Failed to transfer layered shapes document to IGES writer";
            return false;
        }
        
        // Write to file
        if (!caf_writer.Write(file_path.utf8().get_data())) {
            last_error = "Failed to write IGES file with layers";
            return false;
        }

        last_error = "";
        return true;

    } catch (const Standard_Failure& e) {
        last_error = "OpenCASCADE error: " + String(e.GetMessageString());
        ERR_PRINT("IGES layered write error: " + last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = "Standard error: " + String(e.what());
        ERR_PRINT("IGES layered write error: " + last_error);
        return false;
    } catch (...) {
        last_error = "Unknown error occurred during IGES layered writing";
        ERR_PRINT("IGES layered write error: " + last_error);
        return false;
    }
}

bool ocgd_iges_writer::validate_shape_for_iges(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), false, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), false, "Shape is null or invalid");

        // Basic validation
        BRepCheck_Analyzer analyzer(oc_shape);
        if (!analyzer.IsValid()) {
            last_error = "Shape is not valid for IGES export";
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

Dictionary ocgd_iges_writer::get_write_statistics() {
    Dictionary stats;
    stats["last_error"] = last_error;
    stats["system_identifier"] = system_identifier;
    stats["iges_version"] = iges_version;
    stats["write_colors"] = write_colors;
    stats["write_layers"] = write_layers;
    stats["precision_value"] = precision_value;
    stats["model_units"] = model_units;
    return stats;
}

Array ocgd_iges_writer::get_supported_units() {
    Array units;
    units.append("MM"); // millimeters
    units.append("M");  // meters
    units.append("IN"); // inches
    units.append("FT"); // feet
    units.append("MI"); // miles
    units.append("MU"); // microns
    return units;
}

Array ocgd_iges_writer::get_supported_versions() {
    Array versions;
    versions.append(6);  // IGES 1.0
    versions.append(7);  // IGES 2.0
    versions.append(8);  // IGES 3.0
    versions.append(9);  // IGES 4.0
    versions.append(10); // IGES 5.0
    versions.append(11); // IGES 5.3
    return versions;
}

String ocgd_iges_writer::get_last_error() const {
    return last_error;
}

void ocgd_iges_writer::clear_last_error() {
    last_error = "";
}

void ocgd_iges_writer::set_configuration_preset(const String& preset_name) {
    if (preset_name == "CAD_EXPORT") {
        set_write_colors(true);
        set_write_layers(true);
        set_write_names(true);
        set_precision_value(0.01);
        set_write_brep_entities(true);
        set_iges_version(11);
    } else if (preset_name == "MINIMAL") {
        set_write_colors(false);
        set_write_layers(false);
        set_write_names(false);
        set_precision_value(0.1);
        set_write_brep_entities(false);
        set_iges_version(10);
    } else if (preset_name == "HIGH_PRECISION") {
        set_write_colors(true);
        set_write_layers(true);
        set_write_names(true);
        set_precision_value(0.001);
        set_write_brep_entities(true);
        set_iges_version(11);
    }
}

Dictionary ocgd_iges_writer::get_current_configuration() {
    Dictionary config;
    config["system_identifier"] = system_identifier;
    config["preprocessor_version"] = preprocessor_version;
    config["author_name"] = author_name;
    config["organization"] = organization;
    config["description"] = description;
    config["model_space_units"] = model_space_units;
    config["global_scale"] = global_scale;
    config["write_colors"] = write_colors;
    config["write_layers"] = write_layers;
    config["write_names"] = write_names;
    config["validate_on_write"] = validate_on_write;
    config["write_precision_mode"] = write_precision_mode;
    config["precision_value"] = precision_value;
    config["merge_surfaces"] = merge_surfaces;
    config["iges_version"] = iges_version;
    config["drafting_standard"] = drafting_standard;
    config["model_units"] = model_units;
    config["unit_scale_factor"] = unit_scale_factor;
    config["write_brep_entities"] = write_brep_entities;
    config["write_trimmed_surfaces"] = write_trimmed_surfaces;
    config["write_analytical_surfaces"] = write_analytical_surfaces;
    config["optimize_curves"] = optimize_curves;
    config["curve_tolerance"] = curve_tolerance;
    config["surface_tolerance"] = surface_tolerance;
    return config;
}

void ocgd_iges_writer::load_configuration(const Dictionary& config) {
    if (config.has("system_identifier")) set_system_identifier(config["system_identifier"]);
    if (config.has("preprocessor_version")) set_preprocessor_version(config["preprocessor_version"]);
    if (config.has("author_name")) set_author_name(config["author_name"]);
    if (config.has("organization")) set_organization(config["organization"]);
    if (config.has("description")) set_description(config["description"]);
    if (config.has("model_space_units")) set_model_space_units(config["model_space_units"]);
    if (config.has("global_scale")) set_global_scale(config["global_scale"]);
    if (config.has("write_colors")) set_write_colors(config["write_colors"]);
    if (config.has("write_layers")) set_write_layers(config["write_layers"]);
    if (config.has("write_names")) set_write_names(config["write_names"]);
    if (config.has("validate_on_write")) set_validate_on_write(config["validate_on_write"]);
    if (config.has("write_precision_mode")) set_write_precision_mode(config["write_precision_mode"]);
    if (config.has("precision_value")) set_precision_value(config["precision_value"]);
    if (config.has("merge_surfaces")) set_merge_surfaces(config["merge_surfaces"]);
    if (config.has("iges_version")) set_iges_version(config["iges_version"]);
    if (config.has("drafting_standard")) set_drafting_standard(config["drafting_standard"]);
    if (config.has("model_units")) set_model_units(config["model_units"]);
    if (config.has("unit_scale_factor")) set_unit_scale_factor(config["unit_scale_factor"]);
    if (config.has("write_brep_entities")) set_write_brep_entities(config["write_brep_entities"]);
    if (config.has("write_trimmed_surfaces")) set_write_trimmed_surfaces(config["write_trimmed_surfaces"]);
    if (config.has("write_analytical_surfaces")) set_write_analytical_surfaces(config["write_analytical_surfaces"]);
    if (config.has("optimize_curves")) set_optimize_curves(config["optimize_curves"]);
    if (config.has("curve_tolerance")) set_curve_tolerance(config["curve_tolerance"]);
    if (config.has("surface_tolerance")) set_surface_tolerance(config["surface_tolerance"]);
}

Dictionary ocgd_iges_writer::get_iges_file_info(const String& file_path) {
    Dictionary info;
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), info, "File path is empty");

    try {
        // Check if file exists and is readable
        std::ifstream file(file_path.utf8().get_data());
        if (!file.is_open()) {
            info["error"] = "File does not exist or is not readable";
            return info;
        }

        // Read start line to check IGES format
        std::string line;
        if (std::getline(file, line)) {
            // IGES files have specific format with S records in first line
            if (line.length() >= 72 && line.substr(72, 1) == "S") {
                info["is_valid_iges"] = true;
                info["start_line"] = String(line.c_str());
            } else {
                info["is_valid_iges"] = false;
                info["error"] = "Invalid IGES file format";
            }
        }

        file.close();

    } catch (...) {
        info["error"] = "Error reading IGES file";
    }

    return info;
}

bool ocgd_iges_writer::is_valid_iges_file(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    try {
        std::ifstream file(file_path.utf8().get_data());
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        if (std::getline(file, line)) {
            // Check for IGES file format
            if (line.length() >= 72 && line.substr(72, 1) == "S") {
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

void ocgd_iges_writer::set_quality_level(int level) {
    // Quality level affects precision and tolerance settings
    switch (level) {
        case 1: // Low quality
            set_precision_value(0.1);
            set_curve_tolerance(0.1);
            set_surface_tolerance(0.1);
            break;
        case 2: // Medium quality
            set_precision_value(0.01);
            set_curve_tolerance(0.01);
            set_surface_tolerance(0.01);
            break;
        case 3: // High quality
            set_precision_value(0.001);
            set_curve_tolerance(0.001);
            set_surface_tolerance(0.001);
            break;
        default:
            set_precision_value(0.01);
            set_curve_tolerance(0.01);
            set_surface_tolerance(0.01);
            break;
    }
}

int ocgd_iges_writer::get_quality_level() const {
    if (precision_value >= 0.1) return 1;
    else if (precision_value >= 0.01) return 2;
    else return 3;
}

void ocgd_iges_writer::set_angular_tolerance(double tolerance) {
    curve_tolerance = tolerance;
}

double ocgd_iges_writer::get_angular_tolerance() const {
    return curve_tolerance;
}

void ocgd_iges_writer::set_linear_tolerance(double tolerance) {
    precision_value = tolerance;
}

double ocgd_iges_writer::get_linear_tolerance() const {
    return precision_value;
}

void ocgd_iges_writer::set_write_parameter_space_curves(bool enable) {
    // This would be a specific IGES feature - for now, store in optimize_curves
    optimize_curves = enable;
}

bool ocgd_iges_writer::get_write_parameter_space_curves() const {
    return optimize_curves;
}

void ocgd_iges_writer::set_write_isoparametric_curves(bool enable) {
    write_isoparametric_curves = enable;
}

bool ocgd_iges_writer::get_write_isoparametric_curves() const {
    return write_isoparametric_curves;
}

void ocgd_iges_writer::set_convert_surfaces_to_bsplines(bool enable) {
    write_analytical_surfaces = !enable; // Inverse logic
}

bool ocgd_iges_writer::get_convert_surfaces_to_bsplines() const {
    return !write_analytical_surfaces;
}

void ocgd_iges_writer::set_convert_curves_to_bsplines(bool enable) {
    optimize_curves = enable;
}

bool ocgd_iges_writer::get_convert_curves_to_bsplines() const {
    return optimize_curves;
}