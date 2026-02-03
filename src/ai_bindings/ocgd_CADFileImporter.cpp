/**
 * ocgd_CADFileImporter.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE comprehensive CAD file import functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_CADFileImporter.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>

#include <opencascade/STEPCAFControl_Reader.hxx>
#include <opencascade/IGESCAFControl_Reader.hxx>
#include <opencascade/XCAFApp_Application.hxx>
#include <opencascade/TDataStd_Name.hxx>
#include <opencascade/Interface_Static.hxx>
#include <opencascade/StlAPI_Reader.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/ShapeFix_Shape.hxx>
#include <opencascade/Precision.hxx>
#include <opencascade/Standard_Failure.hxx>
#include <opencascade/TColStd_HSequenceOfTransient.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/TCollection_AsciiString.hxx>

#include <algorithm>

void ocgd_CADFileImporter::_bind_methods() {
    // Enums
    BIND_ENUM_CONSTANT(FORMAT_AUTO);
    BIND_ENUM_CONSTANT(FORMAT_STEP);
    BIND_ENUM_CONSTANT(FORMAT_IGES);
    BIND_ENUM_CONSTANT(FORMAT_BREP);
    BIND_ENUM_CONSTANT(FORMAT_STL);
    BIND_ENUM_CONSTANT(FORMAT_OBJ);
    BIND_ENUM_CONSTANT(FORMAT_PLY);

    BIND_ENUM_CONSTANT(MODE_SINGLE_SHAPE);
    BIND_ENUM_CONSTANT(MODE_ASSEMBLY);
    BIND_ENUM_CONSTANT(MODE_SEPARATE_SHAPES);

    BIND_ENUM_CONSTANT(UNIT_MILLIMETER);
    BIND_ENUM_CONSTANT(UNIT_CENTIMETER);
    BIND_ENUM_CONSTANT(UNIT_METER);
    BIND_ENUM_CONSTANT(UNIT_INCH);
    BIND_ENUM_CONSTANT(UNIT_FOOT);
    BIND_ENUM_CONSTANT(UNIT_AUTO);

    BIND_ENUM_CONSTANT(REPAIR_NONE);
    BIND_ENUM_CONSTANT(REPAIR_BASIC);
    BIND_ENUM_CONSTANT(REPAIR_ADVANCED);
    BIND_ENUM_CONSTANT(REPAIR_COMPLETE);

    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_import_format", "format"), &ocgd_CADFileImporter::set_import_format);
    ClassDB::bind_method(D_METHOD("get_import_format"), &ocgd_CADFileImporter::get_import_format);

    ClassDB::bind_method(D_METHOD("set_import_mode", "mode"), &ocgd_CADFileImporter::set_import_mode);
    ClassDB::bind_method(D_METHOD("get_import_mode"), &ocgd_CADFileImporter::get_import_mode);

    ClassDB::bind_method(D_METHOD("set_target_units", "units"), &ocgd_CADFileImporter::set_target_units);
    ClassDB::bind_method(D_METHOD("get_target_units"), &ocgd_CADFileImporter::get_target_units);

    ClassDB::bind_method(D_METHOD("set_repair_level", "level"), &ocgd_CADFileImporter::set_repair_level);
    ClassDB::bind_method(D_METHOD("get_repair_level"), &ocgd_CADFileImporter::get_repair_level);

    ClassDB::bind_method(D_METHOD("set_scaling_factor", "factor"), &ocgd_CADFileImporter::set_scaling_factor);
    ClassDB::bind_method(D_METHOD("get_scaling_factor"), &ocgd_CADFileImporter::get_scaling_factor);

    ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &ocgd_CADFileImporter::set_tolerance);
    ClassDB::bind_method(D_METHOD("get_tolerance"), &ocgd_CADFileImporter::get_tolerance);

    // Processing options
    ClassDB::bind_method(D_METHOD("set_read_colors", "enabled"), &ocgd_CADFileImporter::set_read_colors);
    ClassDB::bind_method(D_METHOD("get_read_colors"), &ocgd_CADFileImporter::get_read_colors);

    ClassDB::bind_method(D_METHOD("set_read_materials", "enabled"), &ocgd_CADFileImporter::set_read_materials);
    ClassDB::bind_method(D_METHOD("get_read_materials"), &ocgd_CADFileImporter::get_read_materials);

    ClassDB::bind_method(D_METHOD("set_read_layers", "enabled"), &ocgd_CADFileImporter::set_read_layers);
    ClassDB::bind_method(D_METHOD("get_read_layers"), &ocgd_CADFileImporter::get_read_layers);

    ClassDB::bind_method(D_METHOD("set_read_metadata", "enabled"), &ocgd_CADFileImporter::set_read_metadata);
    ClassDB::bind_method(D_METHOD("get_read_metadata"), &ocgd_CADFileImporter::get_read_metadata);

    ClassDB::bind_method(D_METHOD("set_read_assembly_structure", "enabled"), &ocgd_CADFileImporter::set_read_assembly_structure);
    ClassDB::bind_method(D_METHOD("get_read_assembly_structure"), &ocgd_CADFileImporter::get_read_assembly_structure);

    ClassDB::bind_method(D_METHOD("set_validate_geometry", "enabled"), &ocgd_CADFileImporter::set_validate_geometry);
    ClassDB::bind_method(D_METHOD("get_validate_geometry"), &ocgd_CADFileImporter::get_validate_geometry);

    ClassDB::bind_method(D_METHOD("set_fix_geometry", "enabled"), &ocgd_CADFileImporter::set_fix_geometry);
    ClassDB::bind_method(D_METHOD("get_fix_geometry"), &ocgd_CADFileImporter::get_fix_geometry);

    // Main import methods
    ClassDB::bind_method(D_METHOD("import_file", "file_path"), &ocgd_CADFileImporter::import_file);
    ClassDB::bind_method(D_METHOD("import_file_multiple", "file_path"), &ocgd_CADFileImporter::import_file_multiple);
    ClassDB::bind_method(D_METHOD("import_file_with_metadata", "file_path"), &ocgd_CADFileImporter::import_file_with_metadata);
    ClassDB::bind_method(D_METHOD("import_files_batch", "file_paths"), &ocgd_CADFileImporter::import_files_batch);

    // File information
    ClassDB::bind_method(D_METHOD("analyze_file", "file_path"), &ocgd_CADFileImporter::analyze_file);
    ClassDB::bind_method(D_METHOD("validate_file", "file_path"), &ocgd_CADFileImporter::validate_file);
    ClassDB::bind_method(D_METHOD("get_supported_extensions", "format"), &ocgd_CADFileImporter::get_supported_extensions);
    ClassDB::bind_method(D_METHOD("detect_format", "file_path"), &ocgd_CADFileImporter::detect_format);

    // Assembly and structure
    ClassDB::bind_method(D_METHOD("get_assembly_structure"), &ocgd_CADFileImporter::get_assembly_structure);
    ClassDB::bind_method(D_METHOD("get_shape_hierarchy"), &ocgd_CADFileImporter::get_shape_hierarchy);
    ClassDB::bind_method(D_METHOD("get_shape_by_name", "name"), &ocgd_CADFileImporter::get_shape_by_name);
    ClassDB::bind_method(D_METHOD("get_shape_names"), &ocgd_CADFileImporter::get_shape_names);

    // Color and material information
    ClassDB::bind_method(D_METHOD("get_color_information"), &ocgd_CADFileImporter::get_color_information);
    ClassDB::bind_method(D_METHOD("get_material_information"), &ocgd_CADFileImporter::get_material_information);
    ClassDB::bind_method(D_METHOD("get_layer_information"), &ocgd_CADFileImporter::get_layer_information);
    ClassDB::bind_method(D_METHOD("get_shape_color", "shape"), &ocgd_CADFileImporter::get_shape_color);
    ClassDB::bind_method(D_METHOD("get_face_colors", "shape"), &ocgd_CADFileImporter::get_face_colors);
    ClassDB::bind_method(D_METHOD("get_shape_material", "shape"), &ocgd_CADFileImporter::get_shape_material);

    // Metadata and properties
    ClassDB::bind_method(D_METHOD("get_file_metadata"), &ocgd_CADFileImporter::get_file_metadata);
    ClassDB::bind_method(D_METHOD("get_shape_properties", "shape"), &ocgd_CADFileImporter::get_shape_properties);
    ClassDB::bind_method(D_METHOD("get_all_properties"), &ocgd_CADFileImporter::get_all_properties);
    ClassDB::bind_method(D_METHOD("get_units_information"), &ocgd_CADFileImporter::get_units_information);

    // Progress and error handling
    ClassDB::bind_method(D_METHOD("get_progress"), &ocgd_CADFileImporter::get_progress);
    ClassDB::bind_method(D_METHOD("cancel_import"), &ocgd_CADFileImporter::cancel_import);
    ClassDB::bind_method(D_METHOD("is_cancelled"), &ocgd_CADFileImporter::is_cancelled);
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_CADFileImporter::get_last_error);
    ClassDB::bind_method(D_METHOD("get_last_warning"), &ocgd_CADFileImporter::get_last_warning);
    ClassDB::bind_method(D_METHOD("clear_messages"), &ocgd_CADFileImporter::clear_messages);

    // Utility methods
    ClassDB::bind_method(D_METHOD("convert_units", "value", "from_units", "to_units"), &ocgd_CADFileImporter::convert_units);
    ClassDB::bind_method(D_METHOD("get_import_statistics"), &ocgd_CADFileImporter::get_import_statistics);
    ClassDB::bind_method(D_METHOD("reset_settings"), &ocgd_CADFileImporter::reset_settings);
}

ocgd_CADFileImporter::ocgd_CADFileImporter() :
    _format(FORMAT_AUTO),
    _import_mode(MODE_SINGLE_SHAPE),
    _target_units(UNIT_AUTO),
    _repair_level(REPAIR_BASIC),
    _read_colors(true),
    _read_materials(true),
    _read_layers(false),
    _read_metadata(true),
    _read_assembly_structure(true),
    _validate_geometry(true),
    _fix_geometry(true),
    _scaling_factor(1.0),
    _tolerance(Precision::Confusion()),
    _progress_current(0),
    _progress_total(100),
    _operation_cancelled(false) {

    initialize_document();
}

ocgd_CADFileImporter::~ocgd_CADFileImporter() {
    cleanup();
}

bool ocgd_CADFileImporter::initialize_document() {
    try {
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("BinXCAF", _document);

        if (_document.IsNull()) {
            set_error("Failed to create XCAF document");
            return false;
        }

        _shape_tool = XCAFDoc_DocumentTool::ShapeTool(_document->Main());
        _color_tool = XCAFDoc_DocumentTool::ColorTool(_document->Main());
        _material_tool = XCAFDoc_DocumentTool::MaterialTool(_document->Main());
        _layer_tool = XCAFDoc_DocumentTool::LayerTool(_document->Main());

        return true;
    } catch (const Standard_Failure& e) {
        set_error(String("Failed to initialize document: ") + String(e.GetMessageString()));
        return false;
    }
}

// Configuration methods
void ocgd_CADFileImporter::set_import_format(ImportFormat format) {
    _format = format;
}

ocgd_CADFileImporter::ImportFormat ocgd_CADFileImporter::get_import_format() const {
    return _format;
}

void ocgd_CADFileImporter::set_import_mode(ImportMode mode) {
    _import_mode = mode;
}

ocgd_CADFileImporter::ImportMode ocgd_CADFileImporter::get_import_mode() const {
    return _import_mode;
}

void ocgd_CADFileImporter::set_target_units(UnitType units) {
    _target_units = units;
}

ocgd_CADFileImporter::UnitType ocgd_CADFileImporter::get_target_units() const {
    return _target_units;
}

void ocgd_CADFileImporter::set_repair_level(RepairLevel level) {
    _repair_level = level;
}

ocgd_CADFileImporter::RepairLevel ocgd_CADFileImporter::get_repair_level() const {
    return _repair_level;
}

void ocgd_CADFileImporter::set_scaling_factor(double factor) {
    _scaling_factor = std::max(0.001, factor);
}

double ocgd_CADFileImporter::get_scaling_factor() const {
    return _scaling_factor;
}

void ocgd_CADFileImporter::set_tolerance(double tolerance) {
    _tolerance = std::max(1e-12, tolerance);
}

double ocgd_CADFileImporter::get_tolerance() const {
    return _tolerance;
}

// Processing options
void ocgd_CADFileImporter::set_read_colors(bool enabled) {
    _read_colors = enabled;
}

bool ocgd_CADFileImporter::get_read_colors() const {
    return _read_colors;
}

void ocgd_CADFileImporter::set_read_materials(bool enabled) {
    _read_materials = enabled;
}

bool ocgd_CADFileImporter::get_read_materials() const {
    return _read_materials;
}

void ocgd_CADFileImporter::set_read_layers(bool enabled) {
    _read_layers = enabled;
}

bool ocgd_CADFileImporter::get_read_layers() const {
    return _read_layers;
}

void ocgd_CADFileImporter::set_read_metadata(bool enabled) {
    _read_metadata = enabled;
}

bool ocgd_CADFileImporter::get_read_metadata() const {
    return _read_metadata;
}

void ocgd_CADFileImporter::set_read_assembly_structure(bool enabled) {
    _read_assembly_structure = enabled;
}

bool ocgd_CADFileImporter::get_read_assembly_structure() const {
    return _read_assembly_structure;
}

void ocgd_CADFileImporter::set_validate_geometry(bool enabled) {
    _validate_geometry = enabled;
}

bool ocgd_CADFileImporter::get_validate_geometry() const {
    return _validate_geometry;
}

void ocgd_CADFileImporter::set_fix_geometry(bool enabled) {
    _fix_geometry = enabled;
}

bool ocgd_CADFileImporter::get_fix_geometry() const {
    return _fix_geometry;
}

// Main import methods
Ref<ocgd_TopoDS_Shape> ocgd_CADFileImporter::import_file(const String& file_path) {
    try {
        clear_messages();
        _operation_cancelled = false;
        update_progress(0, 100);

        if (file_path.is_empty()) {
            UtilityFunctions::printerr("CADFileImporter: Cannot import file - file path is empty");
            set_error("File path is empty");
            return Ref<ocgd_TopoDS_Shape>();
        }

        if (!initialize_document()) {
            UtilityFunctions::printerr("CADFileImporter: Failed to initialize document");
            return Ref<ocgd_TopoDS_Shape>();
        }

        ImportFormat format = (_format == FORMAT_AUTO) ? detect_format(file_path) : _format;

        if (format == FORMAT_AUTO) {
            UtilityFunctions::printerr("CADFileImporter: Could not detect file format for: " + file_path);
            set_error("Could not detect file format");
            return Ref<ocgd_TopoDS_Shape>();
        }

        bool import_success = false;

        switch (format) {
            case FORMAT_STEP:
                import_success = import_step_file(file_path);
                break;
            case FORMAT_IGES:
                import_success = import_iges_file(file_path);
                break;
            case FORMAT_BREP:
                import_success = import_brep_file(file_path);
                break;
            case FORMAT_STL:
                import_success = import_stl_file(file_path);
                break;
            default:
                UtilityFunctions::printerr("CADFileImporter: Unsupported file format");
                set_error("Unsupported file format");
                return Ref<ocgd_TopoDS_Shape>();
        }

        if (!import_success) {
            UtilityFunctions::printerr("CADFileImporter: Import operation failed for: " + file_path);
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Process XCAF document if applicable
        if (format == FORMAT_STEP || format == FORMAT_IGES) {
            try {
                process_xcaf_document();
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("CADFileImporter: Exception processing XCAF document - " + String(e.GetMessageString()));
                set_warning("Failed to process document metadata");
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("CADFileImporter: Exception processing XCAF document - " + String(e.what()));
                set_warning("Failed to process document metadata");
            }
        }

        // Get the first shape
        if (_imported_shapes.size() > 0) {
            update_progress(100, 100);
            return _imported_shapes[0];
        } else {
            UtilityFunctions::printerr("CADFileImporter: No shapes were imported from file: " + file_path);
            set_error("No shapes found in file");
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception importing file '" + file_path + "' - " + String(e.GetMessageString()));
        set_error(String("Import failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception importing file '" + file_path + "' - " + String(e.what()));
        set_error(String("Import failed: ") + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Array ocgd_CADFileImporter::import_file_multiple(const String& file_path) {
    Ref<ocgd_TopoDS_Shape> primary_shape = import_file(file_path);

    if (primary_shape.is_null()) {
        return Array();
    }

    return _imported_shapes;
}

Dictionary ocgd_CADFileImporter::import_file_with_metadata(const String& file_path) {
    Dictionary result;

    Ref<ocgd_TopoDS_Shape> primary_shape = import_file(file_path);

    if (primary_shape.is_null()) {
        result["success"] = false;
        result["error"] = get_last_error();
        return result;
    }

    result["success"] = true;
    result["primary_shape"] = primary_shape;
    result["all_shapes"] = _imported_shapes;

    if (_read_metadata) {
        result["metadata"] = get_file_metadata();
    }

    if (_read_colors) {
        result["colors"] = get_color_information();
    }

    if (_read_materials) {
        result["materials"] = get_material_information();
    }

    if (_read_layers) {
        result["layers"] = get_layer_information();
    }

    if (_read_assembly_structure) {
        result["assembly"] = get_assembly_structure();
    }

    result["import_info"] = _last_import_info;

    return result;
}

Array ocgd_CADFileImporter::import_files_batch(const PackedStringArray& file_paths) {
    Array results;

    try {
        if (file_paths.is_empty()) {
            UtilityFunctions::printerr("CADFileImporter: Cannot import batch - file paths array is empty");
            return results;
        }

        for (int i = 0; i < file_paths.size(); i++) {
            if (should_cancel()) {
                UtilityFunctions::printerr("CADFileImporter: Batch import cancelled at file " + String::num(i));
                break;
            }

            update_progress(i, file_paths.size());

            Dictionary file_result;
            file_result["file_path"] = file_paths[i];

            try {
                Ref<ocgd_TopoDS_Shape> shape = import_file(file_paths[i]);
                if (shape.is_valid()) {
                    file_result["shape"] = shape;
                    file_result["success"] = true;
                } else {
                    file_result["success"] = false;
                    file_result["error"] = get_last_error();
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("CADFileImporter: Exception importing file '" + file_paths[i] + "' - " + String(e.GetMessageString()));
                file_result["success"] = false;
                file_result["error"] = String("Import failed: ") + String(e.GetMessageString());
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("CADFileImporter: Exception importing file '" + file_paths[i] + "' - " + String(e.what()));
                file_result["success"] = false;
                file_result["error"] = String("Import failed: ") + String(e.what());
            }

            results.append(file_result);
        }

        update_progress(file_paths.size(), file_paths.size());
        return results;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception in batch import - " + String(e.GetMessageString()));
        return Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception in batch import - " + String(e.what()));
        return Array();
    }
}

// File information methods
Dictionary ocgd_CADFileImporter::analyze_file(const String& file_path) {
    Dictionary result;

    try {
        if (file_path.is_empty()) {
            UtilityFunctions::printerr("CADFileImporter: Cannot analyze file - file path is empty");
            result["error"] = "File path is empty";
            return result;
        }

        // Basic file information
        result["file_path"] = file_path;
        result["file_exists"] = FileAccess::file_exists(file_path);

        if (!result["file_exists"]) {
            UtilityFunctions::printerr("CADFileImporter: File does not exist: " + file_path);
            result["error"] = "File does not exist";
            return result;
        }

        // Try to detect format
        ImportFormat format = detect_format(file_path);
        result["detected_format"] = format;

        if (format == FORMAT_AUTO) {
            UtilityFunctions::printerr("CADFileImporter: Could not detect format for file: " + file_path);
            result["warning"] = "Could not detect file format";
        }

        // Try to get file size
        try {
            Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
            if (file.is_valid()) {
                result["file_size"] = file->get_length();
                file->close();
            } else {
                UtilityFunctions::printerr("CADFileImporter: Could not open file for size analysis: " + file_path);
                result["warning"] = "Could not read file size";
            }
        } catch (const std::exception& e) {
            UtilityFunctions::printerr("CADFileImporter: Exception reading file size - " + String(e.what()));
            result["warning"] = "Error reading file size";
        }

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception analyzing file '" + file_path + "' - " + String(e.GetMessageString()));
        result["error"] = String("Analysis failed: ") + String(e.GetMessageString());
        return result;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("CADFileImporter: Exception analyzing file '" + file_path + "' - " + String(e.what()));
        result["error"] = String("Analysis failed: ") + String(e.what());
        return result;
    }
}

Dictionary ocgd_CADFileImporter::validate_file(const String& file_path) {
    Dictionary result = analyze_file(file_path);

    if (!result.get("exists", false)) {
        result["valid"] = false;
        return result;
    }

    ImportFormat format = static_cast<ImportFormat>((int)result.get("format", FORMAT_AUTO));
    result["supported_format"] = (format != FORMAT_AUTO);
    result["valid"] = result["supported_format"];

    return result;
}

PackedStringArray ocgd_CADFileImporter::get_supported_extensions(ImportFormat format) const {
    PackedStringArray extensions;

    switch (format) {
        case FORMAT_STEP:
            extensions.append("step");
            extensions.append("stp");
            break;
        case FORMAT_IGES:
            extensions.append("iges");
            extensions.append("igs");
            break;
        case FORMAT_BREP:
            extensions.append("brep");
            break;
        case FORMAT_STL:
            extensions.append("stl");
            break;
        case FORMAT_OBJ:
            extensions.append("obj");
            break;
        case FORMAT_PLY:
            extensions.append("ply");
            break;
        case FORMAT_AUTO:
            extensions.append("step");
            extensions.append("stp");
            extensions.append("iges");
            extensions.append("igs");
            extensions.append("brep");
            extensions.append("stl");
            extensions.append("obj");
            extensions.append("ply");
            break;
    }

    return extensions;
}

ocgd_CADFileImporter::ImportFormat ocgd_CADFileImporter::detect_format(const String& file_path) const {
    return auto_detect_format(file_path);
}

// Assembly and structure methods
Dictionary ocgd_CADFileImporter::get_assembly_structure() const {
    return _assembly_structure;
}

Dictionary ocgd_CADFileImporter::get_shape_hierarchy() const {
    Dictionary hierarchy;

    if (_shape_tool.IsNull()) {
        return hierarchy;
    }

    TDF_LabelSequence free_shapes;
    _shape_tool->GetFreeShapes(free_shapes);

    Array shapes;
    for (int i = 1; i <= free_shapes.Length(); i++) {
        TDF_Label label = free_shapes.Value(i);

        Dictionary shape_info;
        shape_info["label_tag"] = label.Tag();

        // Get shape name if available
        Handle(TDataStd_Name) name;
        if (label.FindAttribute(TDataStd_Name::GetID(), name)) {
            TCollection_AsciiString ascii_name(name->Get());
            shape_info["name"] = String(ascii_name.ToCString());
        }

        TopoDS_Shape shape = _shape_tool->GetShape(label);
        if (!shape.IsNull()) {
            shape_info["has_shape"] = true;
            // Would need to create shape wrapper here
        }

        shapes.append(shape_info);
    }

    hierarchy["shapes"] = shapes;
    return hierarchy;
}

Ref<ocgd_TopoDS_Shape> ocgd_CADFileImporter::get_shape_by_name(const String& name) const {
    // Implementation would search through XCAF labels for matching name
    return Ref<ocgd_TopoDS_Shape>();
}

PackedStringArray ocgd_CADFileImporter::get_shape_names() const {
    PackedStringArray names;

    if (_shape_tool.IsNull()) {
        return names;
    }

    TDF_LabelSequence free_shapes;
    _shape_tool->GetFreeShapes(free_shapes);

    for (int i = 1; i <= free_shapes.Length(); i++) {
        TDF_Label label = free_shapes.Value(i);

        Handle(TDataStd_Name) name;
        if (label.FindAttribute(TDataStd_Name::GetID(), name)) {
            TCollection_AsciiString ascii_name(name->Get());
            names.append(String(ascii_name.ToCString()));
        }
    }

    return names;
}

// Color and material information methods
Dictionary ocgd_CADFileImporter::get_color_information() const {
    return _color_information;
}

Dictionary ocgd_CADFileImporter::get_material_information() const {
    return _material_information;
}

Dictionary ocgd_CADFileImporter::get_layer_information() const {
    Dictionary layer_info;
    // Implementation would extract layer information from XCAF
    return layer_info;
}

Color ocgd_CADFileImporter::get_shape_color(const Ref<ocgd_TopoDS_Shape>& shape) const {
    if (shape.is_null() || _color_tool.IsNull()) {
        return Color(0.7f, 0.7f, 0.7f, 1.0f);
    }

    // Find the shape in XCAF and get its color
    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    Quantity_Color color;

    if (_color_tool->GetColor(occt_shape, XCAFDoc_ColorGen, color)) {
        return quantity_color_to_godot(color);
    }

    return Color(0.7f, 0.7f, 0.7f, 1.0f);
}

PackedColorArray ocgd_CADFileImporter::get_face_colors(const Ref<ocgd_TopoDS_Shape>& shape) const {
    PackedColorArray colors;

    if (shape.is_null()) {
        return colors;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
    while (face_explorer.More()) {
        const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

        Quantity_Color face_color;
        if (_color_tool.IsNull() || !_color_tool->GetColor(face, XCAFDoc_ColorSurf, face_color)) {
            face_color = Quantity_Color(0.7, 0.7, 0.7, Quantity_TOC_RGB);
        }

        colors.append(quantity_color_to_godot(face_color));
        face_explorer.Next();
    }

    return colors;
}

Dictionary ocgd_CADFileImporter::get_shape_material(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary material;

    if (shape.is_null() || _material_tool.IsNull()) {
        return material;
    }

    // Implementation would extract material properties from XCAF
    material["name"] = "Default";
    material["density"] = 1.0;

    return material;
}

// Metadata and properties methods
Dictionary ocgd_CADFileImporter::get_file_metadata() const {
    return _last_import_info;
}

Dictionary ocgd_CADFileImporter::get_shape_properties(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary properties;
    // Implementation would extract custom properties
    return properties;
}

Dictionary ocgd_CADFileImporter::get_all_properties() const {
    Dictionary properties;
    // Implementation would extract all custom properties from file
    return properties;
}

Dictionary ocgd_CADFileImporter::get_units_information() const {
    Dictionary units;
    units["file_units"] = "mm";
    units["target_units"] = "mm";
    units["scaling_factor"] = _scaling_factor;
    return units;
}

// Progress and error handling
int ocgd_CADFileImporter::get_progress() const {
    if (_progress_total == 0) return 0;
    return (100 * _progress_current) / _progress_total;
}

void ocgd_CADFileImporter::cancel_import() {
    _operation_cancelled = true;
}

bool ocgd_CADFileImporter::is_cancelled() const {
    return _operation_cancelled;
}

String ocgd_CADFileImporter::get_last_error() const {
    return _last_error;
}

String ocgd_CADFileImporter::get_last_warning() const {
    return _last_warning;
}

void ocgd_CADFileImporter::clear_messages() {
    _last_error = "";
    _last_warning = "";
}

// Utility methods
double ocgd_CADFileImporter::convert_units(double value, UnitType from_units, UnitType to_units) const {
    if (from_units == to_units) {
        return value;
    }

    // Convert to meters first
    double meters = value;
    switch (from_units) {
        case UNIT_MILLIMETER:
            meters = value / 1000.0;
            break;
        case UNIT_CENTIMETER:
            meters = value / 100.0;
            break;
        case UNIT_METER:
            meters = value;
            break;
        case UNIT_INCH:
            meters = value * 0.0254;
            break;
        case UNIT_FOOT:
            meters = value * 0.3048;
            break;
        default:
            return value;
    }

    // Convert from meters to target
    switch (to_units) {
        case UNIT_MILLIMETER:
            return meters * 1000.0;
        case UNIT_CENTIMETER:
            return meters * 100.0;
        case UNIT_METER:
            return meters;
        case UNIT_INCH:
            return meters / 0.0254;
        case UNIT_FOOT:
            return meters / 0.3048;
        default:
            return meters;
    }
}

Dictionary ocgd_CADFileImporter::get_import_statistics() const {
    Dictionary stats;
    stats["shapes_imported"] = _imported_shapes.size();
    stats["scaling_factor"] = _scaling_factor;
    stats["had_errors"] = !_last_error.is_empty();
    stats["had_warnings"] = !_last_warning.is_empty();
    return stats;
}

void ocgd_CADFileImporter::reset_settings() {
    _format = FORMAT_AUTO;
    _import_mode = MODE_SINGLE_SHAPE;
    _target_units = UNIT_AUTO;
    _repair_level = REPAIR_BASIC;
    _read_colors = true;
    _read_materials = true;
    _read_layers = false;
    _read_metadata = true;
    _read_assembly_structure = true;
    _validate_geometry = true;
    _fix_geometry = true;
    _scaling_factor = 1.0;
    _tolerance = Precision::Confusion();
}

// Private helper methods
ocgd_CADFileImporter::ImportFormat ocgd_CADFileImporter::auto_detect_format(const String& file_path) const {
    String extension = get_file_extension(file_path).to_lower();

    if (extension == "step" || extension == "stp") {
        return FORMAT_STEP;
    } else if (extension == "iges" || extension == "igs") {
        return FORMAT_IGES;
    } else if (extension == "brep") {
        return FORMAT_BREP;
    } else if (extension == "stl") {
        return FORMAT_STL;
    } else if (extension == "obj") {
        return FORMAT_OBJ;
    } else if (extension == "ply") {
        return FORMAT_PLY;
    }

    return FORMAT_AUTO;
}

bool ocgd_CADFileImporter::import_step_file(const String& file_path) {
    try {
        STEPCAFControl_Reader reader;

        IFSelect_ReturnStatus status = reader.ReadFile(file_path.utf8().get_data());
        if (status != IFSelect_RetDone) {
            set_error("Failed to read STEP file");
            return false;
        }

        update_progress(25, 100);

        if (!reader.Transfer(_document)) {
            set_error("Failed to transfer STEP data to document");
            return false;
        }

        update_progress(75, 100);

        return true;

    } catch (const Standard_Failure& e) {
        set_error(String("STEP import failed: ") + String(e.GetMessageString()));
        return false;
    }
}

bool ocgd_CADFileImporter::import_iges_file(const String& file_path) {
    try {
        IGESCAFControl_Reader reader;

        IFSelect_ReturnStatus status = reader.ReadFile(file_path.utf8().get_data());
        if (status != IFSelect_RetDone) {
            set_error("Failed to read IGES file");
            return false;
        }

        update_progress(25, 100);

        if (!reader.Transfer(_document)) {
            set_error("Failed to transfer IGES data to document");
            return false;
        }

        update_progress(75, 100);

        return true;

    } catch (const Standard_Failure& e) {
        set_error(String("IGES import failed: ") + String(e.GetMessageString()));
        return false;
    }
}

bool ocgd_CADFileImporter::import_brep_file(const String& file_path) {
    try {
        BRep_Builder builder;
        TopoDS_Shape shape;

        if (!BRepTools::Read(shape, file_path.utf8().get_data(), builder)) {
            set_error("Failed to read BREP file");
            return false;
        }

        update_progress(50, 100);

        if (!shape.IsNull()) {
            Ref<ocgd_TopoDS_Shape> wrapped_shape = memnew(ocgd_TopoDS_Shape);
            wrapped_shape->set_occt_shape(shape);
            _imported_shapes.append(wrapped_shape);
            update_progress(100, 100);
            return true;
        }

        set_error("Imported BREP shape is null");
        return false;

    } catch (const Standard_Failure& e) {
        set_error(String("BREP import failed: ") + String(e.GetMessageString()));
        return false;
    }
}

bool ocgd_CADFileImporter::import_stl_file(const String& file_path) {
    try {
        StlAPI_Reader reader;
        TopoDS_Shape shape;

        if (!reader.Read(shape, file_path.utf8().get_data())) {
            set_error("Failed to read STL file");
            return false;
        }

        update_progress(50, 100);

        if (!shape.IsNull()) {
            Ref<ocgd_TopoDS_Shape> wrapped_shape = memnew(ocgd_TopoDS_Shape);
            wrapped_shape->set_occt_shape(shape);
            _imported_shapes.append(wrapped_shape);
            update_progress(100, 100);
            return true;
        }

        set_error("Imported STL shape is null");
        return false;

    } catch (const Standard_Failure& e) {
        set_error(String("STL import failed: ") + String(e.GetMessageString()));
        return false;
    }
}

bool ocgd_CADFileImporter::import_obj_file(const String& file_path) {
    set_error("OBJ import not yet implemented");
    return false;
}

bool ocgd_CADFileImporter::import_ply_file(const String& file_path) {
    set_error("PLY import not yet implemented");
    return false;
}

bool ocgd_CADFileImporter::process_xcaf_document() {
    if (_document.IsNull() || _shape_tool.IsNull()) {
        return false;
    }

    try {
        // Extract shapes from XCAF document
        TDF_LabelSequence free_shapes;
        _shape_tool->GetFreeShapes(free_shapes);

        _imported_shapes.clear();

        for (int i = 1; i <= free_shapes.Length(); i++) {
            TDF_Label label = free_shapes.Value(i);
            TopoDS_Shape shape = _shape_tool->GetShape(label);

            if (!shape.IsNull()) {
                Ref<ocgd_TopoDS_Shape> wrapped_shape = memnew(ocgd_TopoDS_Shape);
                wrapped_shape->set_occt_shape(shape);
                _imported_shapes.append(wrapped_shape);
            }
        }

        // Extract additional information
        if (_read_colors) {
            extract_colors();
        }

        if (_read_materials) {
            extract_materials();
        }

        if (_read_assembly_structure) {
            extract_assembly_structure();
        }

        if (_read_metadata) {
            extract_metadata();
        }

        return true;

    } catch (const Standard_Failure& e) {
        set_error(String("XCAF processing failed: ") + String(e.GetMessageString()));
        return false;
    }
}

void ocgd_CADFileImporter::extract_colors() {
    _color_information.clear();

    if (_color_tool.IsNull()) {
        return;
    }

    // Implementation would extract color information from XCAF
    _color_information["has_colors"] = true;
    _color_information["default_color"] = Color(0.7f, 0.7f, 0.7f, 1.0f);
}

void ocgd_CADFileImporter::extract_materials() {
    _material_information.clear();

    if (_material_tool.IsNull()) {
        return;
    }

    // Implementation would extract material information from XCAF
    _material_information["has_materials"] = true;
}

void ocgd_CADFileImporter::extract_layers() {
    // Implementation would extract layer information from XCAF
}

void ocgd_CADFileImporter::extract_assembly_structure() {
    _assembly_structure.clear();

    if (_shape_tool.IsNull()) {
        return;
    }

    // Implementation would extract assembly hierarchy from XCAF
    _assembly_structure["has_assembly"] = true;
    _assembly_structure["root_shapes"] = _imported_shapes.size();
}

void ocgd_CADFileImporter::extract_metadata() {
    _last_import_info.clear();

    _last_import_info["import_format"] = _format;
    _last_import_info["shapes_count"] = _imported_shapes.size();
    _last_import_info["has_colors"] = _read_colors;
    _last_import_info["has_materials"] = _read_materials;
    _last_import_info["scaling_applied"] = (_scaling_factor != 1.0);
}

Color ocgd_CADFileImporter::quantity_color_to_godot(const Quantity_Color& occt_color) const {
    return Color(
        static_cast<float>(occt_color.Red()),
        static_cast<float>(occt_color.Green()),
        static_cast<float>(occt_color.Blue()),
        1.0f
    );
}

Color ocgd_CADFileImporter::quantity_color_rgba_to_godot(const Quantity_ColorRGBA& occt_color) const {
    return Color(
        static_cast<float>(occt_color.GetRGB().Red()),
        static_cast<float>(occt_color.GetRGB().Green()),
        static_cast<float>(occt_color.GetRGB().Blue()),
        static_cast<float>(occt_color.Alpha())
    );
}

String ocgd_CADFileImporter::get_file_extension(const String& file_path) const {
    int dot_pos = file_path.rfind(".");
    if (dot_pos >= 0) {
        return file_path.substr(dot_pos + 1);
    }
    return "";
}

void ocgd_CADFileImporter::update_progress(int current, int total) const {
    _progress_current = current;
    _progress_total = total;
}

void ocgd_CADFileImporter::set_error(const String& error) const {
    _last_error = error;
    UtilityFunctions::printerr("CADFileImporter: " + error);
}

void ocgd_CADFileImporter::set_warning(const String& warning) const {
    _last_warning = warning;
    UtilityFunctions::print("CADFileImporter Warning: " + warning);
}

bool ocgd_CADFileImporter::should_cancel() const {
    return _operation_cancelled;
}

void ocgd_CADFileImporter::cleanup() {
    _imported_shapes.clear();
    _assembly_structure.clear();
    _color_information.clear();
    _material_information.clear();
    _last_import_info.clear();
}
