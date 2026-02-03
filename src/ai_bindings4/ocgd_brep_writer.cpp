#include "ocgd_brep_writer.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>

// OpenCASCADE includes
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <Standard_IStream.hxx>
#include <Standard_OStream.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <ShapeFix_Shape.hxx>
#include <Precision.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <Standard_Failure.hxx>
#include <sstream>

using namespace godot;

ocgd_brep_writer::ocgd_brep_writer() {
    write_triangulation = true;
    write_curves = true;
    write_surfaces = true;
    precision_tolerance = Precision::Confusion();
    brep_version = "7.0";
    binary_format = false;
    compress_output = false;
    shape_optimization = false;
    last_error = "";
}

ocgd_brep_writer::~ocgd_brep_writer() {
}

void ocgd_brep_writer::_bind_methods() {
    ClassDB::bind_static_method("ocgd_brep_writer", D_METHOD("new_writer"), &ocgd_brep_writer::new_writer);

    ClassDB::bind_method(D_METHOD("write_file", "shape", "file_path"), &ocgd_brep_writer::write_file);
    ClassDB::bind_method(D_METHOD("write_shapes", "shapes", "file_path"), &ocgd_brep_writer::write_shapes);
    ClassDB::bind_method(D_METHOD("write_file_with_options", "shape", "file_path", "options"), &ocgd_brep_writer::write_file_with_options);
    ClassDB::bind_method(D_METHOD("write_shapes_with_options", "shapes", "file_path", "options"), &ocgd_brep_writer::write_shapes_with_options);
    ClassDB::bind_method(D_METHOD("write_to_string", "shape"), &ocgd_brep_writer::write_to_string);
    ClassDB::bind_method(D_METHOD("write_shapes_to_string", "shapes"), &ocgd_brep_writer::write_shapes_to_string);

    ClassDB::bind_method(D_METHOD("set_write_triangulation", "enable"), &ocgd_brep_writer::set_write_triangulation);
    ClassDB::bind_method(D_METHOD("get_write_triangulation"), &ocgd_brep_writer::get_write_triangulation);
    ClassDB::bind_method(D_METHOD("set_write_curves", "enable"), &ocgd_brep_writer::set_write_curves);
    ClassDB::bind_method(D_METHOD("get_write_curves"), &ocgd_brep_writer::get_write_curves);
    ClassDB::bind_method(D_METHOD("set_write_surfaces", "enable"), &ocgd_brep_writer::set_write_surfaces);
    ClassDB::bind_method(D_METHOD("get_write_surfaces"), &ocgd_brep_writer::get_write_surfaces);
    ClassDB::bind_method(D_METHOD("set_precision_tolerance", "tolerance"), &ocgd_brep_writer::set_precision_tolerance);
    ClassDB::bind_method(D_METHOD("get_precision_tolerance"), &ocgd_brep_writer::get_precision_tolerance);
    ClassDB::bind_method(D_METHOD("set_brep_version", "version"), &ocgd_brep_writer::set_brep_version);
    ClassDB::bind_method(D_METHOD("get_brep_version"), &ocgd_brep_writer::get_brep_version);
    ClassDB::bind_method(D_METHOD("set_binary_format", "binary"), &ocgd_brep_writer::set_binary_format);
    ClassDB::bind_method(D_METHOD("get_binary_format"), &ocgd_brep_writer::get_binary_format);
    ClassDB::bind_method(D_METHOD("set_compress_output", "compress"), &ocgd_brep_writer::set_compress_output);
    ClassDB::bind_method(D_METHOD("get_compress_output"), &ocgd_brep_writer::get_compress_output);

    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_brep_writer::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_brep_writer::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_brep_writer::clear_error);

    ClassDB::bind_method(D_METHOD("validate_shape_for_export", "shape"), &ocgd_brep_writer::validate_shape_for_export);
    ClassDB::bind_method(D_METHOD("validate_shapes_for_export", "shapes"), &ocgd_brep_writer::validate_shapes_for_export);
    ClassDB::bind_method(D_METHOD("get_supported_extensions"), &ocgd_brep_writer::get_supported_extensions);

    ClassDB::bind_method(D_METHOD("get_export_info"), &ocgd_brep_writer::get_export_info);
    ClassDB::bind_method(D_METHOD("get_export_statistics"), &ocgd_brep_writer::get_export_statistics);
    ClassDB::bind_method(D_METHOD("get_exported_shape_count"), &ocgd_brep_writer::get_exported_shape_count);
    ClassDB::bind_method(D_METHOD("get_export_file_size"), &ocgd_brep_writer::get_export_file_size);

    ClassDB::bind_method(D_METHOD("can_export_shape_type", "shape_type"), &ocgd_brep_writer::can_export_shape_type);
    ClassDB::bind_method(D_METHOD("get_supported_shape_types"), &ocgd_brep_writer::get_supported_shape_types);

    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_brep_writer::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_brep_writer::is_null);

    ClassDB::bind_method(D_METHOD("get_supported_brep_versions"), &ocgd_brep_writer::get_supported_brep_versions);
    ClassDB::bind_method(D_METHOD("is_version_compatible", "version"), &ocgd_brep_writer::is_version_compatible);
    ClassDB::bind_method(D_METHOD("get_recommended_version"), &ocgd_brep_writer::get_recommended_version);
}

Ref<ocgd_brep_writer> ocgd_brep_writer::new_writer() {
    return memnew(ocgd_brep_writer);
}

bool ocgd_brep_writer::write_file(const Ref<ocgd_shape>& shape, const String& file_path) {
    Dictionary options;
    return write_file_with_options(shape, file_path, options);
}

bool ocgd_brep_writer::write_shapes(const Array& shapes, const String& file_path) {
    Dictionary options;
    return write_shapes_with_options(shapes, file_path, options);
}

bool ocgd_brep_writer::write_file_with_options(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    ERR_FAIL_NULL_V_MSG(shape.ptr(), false, "Shape reference is null");

    if (shape.is_null()) {
        last_error = "Cannot write null shape to BREP file";
        ERR_PRINT(last_error);
        return false;
    }

    TopoDS_Shape occ_shape = shape->get_shape();
    if (occ_shape.IsNull()) {
        last_error = "Cannot write null OpenCASCADE shape to BREP file";
        ERR_PRINT(last_error);
        return false;
    }

    try {
        // Process options
        if (options.has("precision_tolerance")) {
            precision_tolerance = options["precision_tolerance"];
        }

        if (options.has("write_triangulation")) {
            write_triangulation = options["write_triangulation"];
        }

        if (options.has("write_curves")) {
            write_curves = options["write_curves"];
        }

        if (options.has("write_surfaces")) {
            write_surfaces = options["write_surfaces"];
        }

        if (options.has("binary_format")) {
            binary_format = options["binary_format"];
        }

        // Validate shape before export
        if (!validate_shape_for_export(shape)) {
            return false;
        }

        // Apply shape fixes if needed
        TopoDS_Shape export_shape = occ_shape;
        if (options.has("fix_shapes_before_export") && bool(options["fix_shapes_before_export"])) {
            Handle(ShapeFix_Shape) shape_fixer = new ShapeFix_Shape(occ_shape);
            shape_fixer->SetPrecision(precision_tolerance);
            shape_fixer->Perform();
            export_shape = shape_fixer->Shape();
        }

        // Write the BREP file
        CharString path_utf8 = file_path.utf8();
        bool success = BRepTools::Write(export_shape, path_utf8.get_data());

        if (!success) {
            last_error = String("Failed to write BREP file: ") + file_path;
            return false;
        }

        return true;

    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during BREP export: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = String("Standard exception during BREP export: ") + e.what();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        last_error = "Unknown exception during BREP export";
        ERR_PRINT(last_error);
        return false;
    }
}

bool ocgd_brep_writer::write_shapes_with_options(const Array& shapes, const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");

    if (shapes.size() == 0) {
        last_error = "Cannot write empty shapes array to BREP file";
        ERR_PRINT(last_error);
        return false;
    }

    try {
        // Create compound shape from array
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            if (shape.is_null()) {
                last_error = String("Shape at index ") + String::num(i) + " is null";
                ERR_PRINT(last_error);
                return false;
            }

            TopoDS_Shape occ_shape = shape->get_shape();
            if (occ_shape.IsNull()) {
                last_error = String("OpenCASCADE shape at index ") + String::num(i) + " is null";
                ERR_PRINT(last_error);
                return false;
            }

            builder.Add(compound, occ_shape);
        }

        // Create temporary shape wrapper for the compound
        Ref<ocgd_shape> compound_shape = ocgd_shape::new_shape();
        compound_shape->set_shape(compound);

        // Use the single shape export method
        return write_file_with_options(compound_shape, file_path, options);

    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during multi-shape BREP export: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = String("Standard exception during multi-shape BREP export: ") + e.what();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        last_error = "Unknown exception during multi-shape BREP export";
        ERR_PRINT(last_error);
        return false;
    }
}

String ocgd_brep_writer::write_to_string(const Ref<ocgd_shape>& shape) {
    clear_error();
    ERR_FAIL_NULL_V_MSG(shape.ptr(), "", "Shape reference is null");

    if (shape.is_null()) {
        last_error = "Cannot write null shape to string";
        ERR_PRINT(last_error);
        return "";
    }

    TopoDS_Shape occ_shape = shape->get_shape();
    if (occ_shape.IsNull()) {
        last_error = "Cannot write null OpenCASCADE shape to string";
        ERR_PRINT(last_error);
        return "";
    }

    try {
        std::ostringstream stream;
        BRepTools::Write(occ_shape, stream);
        return String(stream.str().c_str());

    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during BREP string export: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return "";
    } catch (const std::exception& e) {
        last_error = String("Standard exception during BREP string export: ") + e.what();
        ERR_PRINT(last_error);
        return "";
    } catch (...) {
        last_error = "Unknown exception during BREP string export";
        ERR_PRINT(last_error);
        return "";
    }
}

String ocgd_brep_writer::write_shapes_to_string(const Array& shapes) {
    if (shapes.size() == 0) {
        last_error = "Cannot write empty shapes array to string";
        ERR_PRINT(last_error);
        return "";
    }

    if (shapes.size() == 1) {
        return write_to_string(shapes[0]);
    }

    try {
        // Create compound from multiple shapes
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_shape> shape = shapes[i];
            if (!shape.is_null() && !shape->get_shape().IsNull()) {
                builder.Add(compound, shape->get_shape());
            }
        }

        // Create temporary shape wrapper
        Ref<ocgd_shape> compound_shape = ocgd_shape::new_shape();
        compound_shape->set_shape(compound);

        return write_to_string(compound_shape);

    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during multi-shape string export: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return "";
    } catch (const std::exception& e) {
        last_error = String("Standard exception during multi-shape string export: ") + e.what();
        ERR_PRINT(last_error);
        return "";
    } catch (...) {
        last_error = "Unknown exception during multi-shape string export";
        ERR_PRINT(last_error);
        return "";
    }
}

void ocgd_brep_writer::set_write_triangulation(bool enable) {
    write_triangulation = enable;
}

bool ocgd_brep_writer::get_write_triangulation() const {
    return write_triangulation;
}

void ocgd_brep_writer::set_write_curves(bool enable) {
    write_curves = enable;
}

bool ocgd_brep_writer::get_write_curves() const {
    return write_curves;
}

void ocgd_brep_writer::set_write_surfaces(bool enable) {
    write_surfaces = enable;
}

bool ocgd_brep_writer::get_write_surfaces() const {
    return write_surfaces;
}

void ocgd_brep_writer::set_precision_tolerance(double tolerance) {
    precision_tolerance = tolerance;
}

double ocgd_brep_writer::get_precision_tolerance() const {
    return precision_tolerance;
}

void ocgd_brep_writer::set_brep_version(const String& version) {
    brep_version = version;
}

String ocgd_brep_writer::get_brep_version() const {
    return brep_version;
}

void ocgd_brep_writer::set_binary_format(bool binary) {
    binary_format = binary;
}

bool ocgd_brep_writer::get_binary_format() const {
    return binary_format;
}

void ocgd_brep_writer::set_compress_output(bool compress) {
    compress_output = compress;
}

bool ocgd_brep_writer::get_compress_output() const {
    return compress_output;
}

String ocgd_brep_writer::get_last_error() const {
    return last_error;
}

bool ocgd_brep_writer::has_error() const {
    return !last_error.is_empty();
}

void ocgd_brep_writer::clear_error() {
    last_error = "";
}

bool ocgd_brep_writer::validate_shape_for_export(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), false, "Shape reference is null");

    if (shape.is_null()) {
        last_error = "Shape is null";
        ERR_PRINT(last_error);
        return false;
    }

    TopoDS_Shape occ_shape = shape->get_shape();
    if (occ_shape.IsNull()) {
        last_error = "OpenCASCADE shape is null";
        ERR_PRINT(last_error);
        return false;
    }

    try {
        BRepCheck_Analyzer analyzer(occ_shape);
        if (!analyzer.IsValid()) {
            last_error = "Shape contains invalid geometry";
            return false;
        }

        return true;

    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during shape validation: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return false;
    } catch (const std::exception& e) {
        last_error = String("Standard exception during shape validation: ") + e.what();
        ERR_PRINT(last_error);
        return false;
    } catch (...) {
        last_error = "Unknown exception during shape validation";
        ERR_PRINT(last_error);
        return false;
    }
}

bool ocgd_brep_writer::validate_shapes_for_export(const Array& shapes) {
    if (shapes.size() == 0) {
        last_error = "No shapes provided for validation";
        ERR_PRINT(last_error);
        return false;
    }

    for (int i = 0; i < shapes.size(); i++) {
        if (!validate_shape_for_export(shapes[i])) {
            last_error = String("Shape at index ") + String::num(i) + " failed validation: " + last_error;
            ERR_PRINT(last_error);
            return false;
        }
    }

    return true;
}

Array ocgd_brep_writer::get_supported_extensions() const {
    Array extensions;
    extensions.append("brep");
    extensions.append("brp");
    return extensions;
}

Dictionary ocgd_brep_writer::get_export_info() const {
    Dictionary info;
    info["type"] = "BREP Writer";
    info["last_error"] = last_error;
    info["write_triangulation"] = write_triangulation;
    info["write_curves"] = write_curves;
    info["write_surfaces"] = write_surfaces;
    info["precision_tolerance"] = precision_tolerance;
    info["brep_version"] = brep_version;
    info["binary_format"] = binary_format;
    info["compress_output"] = compress_output;
    return info;
}

Dictionary ocgd_brep_writer::get_export_statistics() const {
    Dictionary stats;
    stats["type"] = "BREP Export Statistics";
    return stats;
}

int ocgd_brep_writer::get_exported_shape_count() const {
    return 0; // Would need to track during export
}

double ocgd_brep_writer::get_export_file_size() const {
    return 0.0; // Would need to track during export
}

bool ocgd_brep_writer::can_export_shape_type(const String& shape_type) {
    return true; // BREP can handle all OpenCASCADE shape types
}

Array ocgd_brep_writer::get_supported_shape_types() const {
    Array types;
    types.append("vertex");
    types.append("edge");
    types.append("wire");
    types.append("face");
    types.append("shell");
    types.append("solid");
    types.append("compound");
    return types;
}

String ocgd_brep_writer::get_type() const {
    return "ocgd_brep_writer";
}

bool ocgd_brep_writer::is_null() const {
    return false;
}

Array ocgd_brep_writer::get_supported_brep_versions() const {
    Array versions;
    versions.append("6.0");
    versions.append("6.1");
    versions.append("6.2");
    versions.append("6.3");
    versions.append("6.4");
    versions.append("6.5");
    versions.append("6.6");
    versions.append("6.7");
    versions.append("6.8");
    versions.append("6.9");
    versions.append("7.0");
    versions.append("7.1");
    versions.append("7.2");
    versions.append("7.3");
    versions.append("7.4");
    versions.append("7.5");
    versions.append("7.6");
    versions.append("7.7");
    return versions;
}

bool ocgd_brep_writer::is_version_compatible(const String& version) const {
    Array supported = get_supported_brep_versions();
    return supported.has(version);
}

String ocgd_brep_writer::get_recommended_version() const {
    return "7.0";
}

// Shape optimization methods
void ocgd_brep_writer::set_shape_optimization(bool enable) {
    shape_optimization = enable;
}

bool ocgd_brep_writer::get_shape_optimization() const {
    return shape_optimization;
}
void ocgd_brep_writer::set_remove_duplicates(bool enable) {}
bool ocgd_brep_writer::get_remove_duplicates() const { return false; }
void ocgd_brep_writer::set_merge_vertices(bool enable) {}
bool ocgd_brep_writer::get_merge_vertices() const { return false; }
void ocgd_brep_writer::set_fix_shapes_before_export(bool enable) {}
bool ocgd_brep_writer::get_fix_shapes_before_export() const { return false; }
bool ocgd_brep_writer::perform_shape_check_before_export(const Ref<ocgd_shape>& shape) { return validate_shape_for_export(shape); }
Dictionary ocgd_brep_writer::get_shape_quality_report(const Ref<ocgd_shape>& shape) { return Dictionary(); }
bool ocgd_brep_writer::repair_shape_for_export(Ref<ocgd_shape>& shape) { return false; }
bool ocgd_brep_writer::write_multiple_files(const Array& shapes, const Array& file_paths) { return false; }
bool ocgd_brep_writer::write_assembly_as_separate_files(const Ref<ocgd_shape>& assembly, const String& base_path) { return false; }
void ocgd_brep_writer::set_export_metadata(const Dictionary& metadata) {}
Dictionary ocgd_brep_writer::get_export_metadata() const { return Dictionary(); }
void ocgd_brep_writer::add_export_comment(const String& comment) {}
void ocgd_brep_writer::clear_export_comments() {}
void ocgd_brep_writer::set_progress_callback_enabled(bool enable) {}
bool ocgd_brep_writer::get_progress_callback_enabled() const { return false; }
double ocgd_brep_writer::get_export_progress() const { return 0.0; }
void ocgd_brep_writer::set_memory_limit(int limit_mb) {}
int ocgd_brep_writer::get_memory_limit() const { return 0; }
void ocgd_brep_writer::set_streaming_mode(bool enable) {}
bool ocgd_brep_writer::get_streaming_mode() const { return false; }
