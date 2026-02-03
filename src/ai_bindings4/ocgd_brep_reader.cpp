#include "ocgd_brep_reader.h"
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
#include <chrono>
#include <fstream>
#include <sstream>

using namespace godot;

ocgd_brep_reader::ocgd_brep_reader() {
    load_triangulation = true;
    load_curves = true;
    load_surfaces = true;
    precision_tolerance = Precision::Confusion();
    merge_vertices = false;
    fix_shapes = false;
    memory_limit = 0;
    load_time = 0.0;
    memory_used = 0;
    shapes_loaded = 0;
    last_error = "";
}

ocgd_brep_reader::~ocgd_brep_reader() {
}

void ocgd_brep_reader::_bind_methods() {
    ClassDB::bind_static_method("ocgd_brep_reader", D_METHOD("new_reader"), &ocgd_brep_reader::new_reader);
    
    ClassDB::bind_method(D_METHOD("load_file", "file_path"), &ocgd_brep_reader::load_file);
    ClassDB::bind_method(D_METHOD("load_file_with_options", "file_path", "options"), &ocgd_brep_reader::load_file_with_options);
    ClassDB::bind_method(D_METHOD("load_from_string", "brep_content"), &ocgd_brep_reader::load_from_string);
    ClassDB::bind_method(D_METHOD("get_file_info"), &ocgd_brep_reader::get_file_info);
    ClassDB::bind_method(D_METHOD("get_all_shapes"), &ocgd_brep_reader::get_all_shapes);
    
    ClassDB::bind_method(D_METHOD("set_load_triangulation", "enable"), &ocgd_brep_reader::set_load_triangulation);
    ClassDB::bind_method(D_METHOD("get_load_triangulation"), &ocgd_brep_reader::get_load_triangulation);
    ClassDB::bind_method(D_METHOD("set_load_curves", "enable"), &ocgd_brep_reader::set_load_curves);
    ClassDB::bind_method(D_METHOD("get_load_curves"), &ocgd_brep_reader::get_load_curves);
    ClassDB::bind_method(D_METHOD("set_load_surfaces", "enable"), &ocgd_brep_reader::set_load_surfaces);
    ClassDB::bind_method(D_METHOD("get_load_surfaces"), &ocgd_brep_reader::get_load_surfaces);
    ClassDB::bind_method(D_METHOD("set_precision_tolerance", "tolerance"), &ocgd_brep_reader::set_precision_tolerance);
    ClassDB::bind_method(D_METHOD("get_precision_tolerance"), &ocgd_brep_reader::get_precision_tolerance);
    
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_brep_reader::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_brep_reader::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_brep_reader::clear_error);
    
    ClassDB::bind_method(D_METHOD("validate_file", "file_path"), &ocgd_brep_reader::validate_file);
    ClassDB::bind_method(D_METHOD("validate_string", "brep_content"), &ocgd_brep_reader::validate_string);
    ClassDB::bind_method(D_METHOD("get_supported_extensions"), &ocgd_brep_reader::get_supported_extensions);
    
    ClassDB::bind_method(D_METHOD("analyze_shape", "shape"), &ocgd_brep_reader::analyze_shape);
    ClassDB::bind_method(D_METHOD("get_shape_complexity", "shape"), &ocgd_brep_reader::get_shape_complexity);
    
    ClassDB::bind_method(D_METHOD("is_brep_file", "file_path"), &ocgd_brep_reader::is_brep_file);
    ClassDB::bind_method(D_METHOD("detect_brep_version", "file_path"), &ocgd_brep_reader::detect_brep_version);
    
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_brep_reader::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_brep_reader::is_null);
    
    ClassDB::bind_method(D_METHOD("set_merge_vertices", "enable"), &ocgd_brep_reader::set_merge_vertices);
    ClassDB::bind_method(D_METHOD("get_merge_vertices"), &ocgd_brep_reader::get_merge_vertices);
    ClassDB::bind_method(D_METHOD("set_fix_shapes", "enable"), &ocgd_brep_reader::set_fix_shapes);
    ClassDB::bind_method(D_METHOD("get_fix_shapes"), &ocgd_brep_reader::get_fix_shapes);
    
    ClassDB::bind_method(D_METHOD("set_memory_limit", "limit_mb"), &ocgd_brep_reader::set_memory_limit);
    ClassDB::bind_method(D_METHOD("get_memory_limit"), &ocgd_brep_reader::get_memory_limit);
    
    ClassDB::bind_method(D_METHOD("get_load_statistics"), &ocgd_brep_reader::get_load_statistics);
    ClassDB::bind_method(D_METHOD("get_load_time"), &ocgd_brep_reader::get_load_time);
}

Ref<ocgd_brep_reader> ocgd_brep_reader::new_reader() {
    return memnew(ocgd_brep_reader);
}

Ref<ocgd_shape> ocgd_brep_reader::load_file(const String& file_path) {
    Dictionary options;
    return load_file_with_options(file_path, options);
}

Ref<ocgd_shape> ocgd_brep_reader::load_file_with_options(const String& file_path, const Dictionary& options) {
    clear_error();
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), Ref<ocgd_shape>(), "File path is empty");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    try {
        // Process options
        if (options.has("precision_tolerance")) {
            precision_tolerance = options["precision_tolerance"];
        }
        
        if (options.has("load_triangulation")) {
            load_triangulation = options["load_triangulation"];
        }
        
        if (options.has("load_curves")) {
            load_curves = options["load_curves"];
        }
        
        if (options.has("load_surfaces")) {
            load_surfaces = options["load_surfaces"];
        }
        
        // Check if file exists and is readable
        CharString path_utf8 = file_path.utf8();
        std::ifstream test_file(path_utf8.get_data());
        if (!test_file.is_open()) {
            last_error = String("Cannot open BREP file: ") + file_path;
            return Ref<ocgd_shape>();
        }
        test_file.close();
        
        // Load the BREP file
        TopoDS_Shape loaded_shape;
        BRep_Builder builder;
        
        bool success = BRepTools::Read(loaded_shape, path_utf8.get_data(), builder);
        
        if (!success || loaded_shape.IsNull()) {
            last_error = String("Failed to read BREP file: ") + file_path;
            return Ref<ocgd_shape>();
        }
        
        // Validate the loaded shape
        BRepCheck_Analyzer analyzer(loaded_shape);
        if (!analyzer.IsValid()) {
            if (options.has("fix_shapes") && bool(options["fix_shapes"])) {
                // Try to fix the shape
                Handle(ShapeFix_Shape) shape_fixer = new ShapeFix_Shape(loaded_shape);
                shape_fixer->SetPrecision(precision_tolerance);
                shape_fixer->Perform();
                loaded_shape = shape_fixer->Shape();
                
                // Re-validate
                BRepCheck_Analyzer re_analyzer(loaded_shape);
                if (!re_analyzer.IsValid()) {
                    last_error = "BREP file contains invalid geometry that could not be fixed";
                    return Ref<ocgd_shape>();
                }
            } else {
                last_error = "BREP file contains invalid geometry";
                return Ref<ocgd_shape>();
            }
        }
        
        // Apply additional processing if requested
        if (options.has("merge_vertices") && bool(options["merge_vertices"])) {
            BRepBuilderAPI_Sewing sewing(precision_tolerance);
            sewing.Add(loaded_shape);
            sewing.Perform();
            loaded_shape = sewing.SewedShape();
        }
        
        // Create and return ocgd_shape wrapper
        Ref<ocgd_shape> shape_wrapper = ocgd_shape::new_shape();
        shape_wrapper->set_shape(loaded_shape);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return shape_wrapper;
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during BREP import: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (const std::exception& e) {
        last_error = String("Standard exception during BREP import: ") + e.what();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (...) {
        last_error = "Unknown exception during BREP import";
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    }
}

Ref<ocgd_shape> ocgd_brep_reader::load_from_string(const String& brep_content) {
    clear_error();
    ERR_FAIL_COND_V_MSG(brep_content.is_empty(), Ref<ocgd_shape>(), "BREP content is empty");
    
    try {
        CharString content_utf8 = brep_content.utf8();
        std::istringstream stream(content_utf8.get_data());
        
        TopoDS_Shape loaded_shape;
        BRep_Builder builder;
        
        BRepTools::Read(loaded_shape, stream, builder);
        
        if (loaded_shape.IsNull()) {
            last_error = "Failed to parse BREP content from string";
            return Ref<ocgd_shape>();
        }
        
        // Validate the loaded shape
        BRepCheck_Analyzer analyzer(loaded_shape);
        if (!analyzer.IsValid()) {
            last_error = "BREP content contains invalid geometry";
            return Ref<ocgd_shape>();
        }
        
        // Create and return ocgd_shape wrapper
        Ref<ocgd_shape> shape_wrapper = ocgd_shape::new_shape();
        shape_wrapper->set_shape(loaded_shape);
        
        return shape_wrapper;
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error during BREP string parsing: ") + e.GetMessageString();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (const std::exception& e) {
        last_error = String("Standard exception during BREP string parsing: ") + e.what();
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    } catch (...) {
        last_error = "Unknown exception during BREP string parsing";
        ERR_PRINT(last_error);
        return Ref<ocgd_shape>();
    }
}

Dictionary ocgd_brep_reader::get_file_info() const {
    Dictionary info;
    info["type"] = "BREP Reader";
    info["last_error"] = last_error;
    info["load_triangulation"] = load_triangulation;
    info["load_curves"] = load_curves;
    info["load_surfaces"] = load_surfaces;
    info["precision_tolerance"] = precision_tolerance;
    return info;
}

Array ocgd_brep_reader::get_all_shapes() {
    Array shapes;
    // For BREP files, typically contains a single shape
    // This would require storing the loaded content for analysis
    return shapes;
}

void ocgd_brep_reader::set_load_triangulation(bool enable) {
    load_triangulation = enable;
}

bool ocgd_brep_reader::get_load_triangulation() const {
    return load_triangulation;
}

void ocgd_brep_reader::set_load_curves(bool enable) {
    load_curves = enable;
}

bool ocgd_brep_reader::get_load_curves() const {
    return load_curves;
}

void ocgd_brep_reader::set_load_surfaces(bool enable) {
    load_surfaces = enable;
}

bool ocgd_brep_reader::get_load_surfaces() const {
    return load_surfaces;
}

void ocgd_brep_reader::set_precision_tolerance(double tolerance) {
    precision_tolerance = tolerance;
}

double ocgd_brep_reader::get_precision_tolerance() const {
    return precision_tolerance;
}

String ocgd_brep_reader::get_last_error() const {
    return last_error;
}

bool ocgd_brep_reader::has_error() const {
    return !last_error.is_empty();
}

void ocgd_brep_reader::clear_error() {
    last_error = "";
}

bool ocgd_brep_reader::validate_file(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    
    CharString path_utf8 = file_path.utf8();
    
    // Check extension
    String ext = file_path.get_extension().to_lower();
    if (ext != "brep" && ext != "brp") {
        return false;
    }
    
    // Try to read the file
    try {
        TopoDS_Shape test_shape;
        BRep_Builder builder;
        return BRepTools::Read(test_shape, path_utf8.get_data(), builder) && !test_shape.IsNull();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error validating file: ") + e.GetMessageString();
        return false;
    } catch (...) {
        return false;
    }
}

bool ocgd_brep_reader::validate_string(const String& brep_content) {
    ERR_FAIL_COND_V_MSG(brep_content.is_empty(), false, "BREP content is empty");
    
    try {
        CharString content_utf8 = brep_content.utf8();
        std::istringstream stream(content_utf8.get_data());
        
        TopoDS_Shape test_shape;
        BRep_Builder builder;
        BRepTools::Read(test_shape, stream, builder);
        return !test_shape.IsNull();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error validating string: ") + e.GetMessageString();
        return false;
    } catch (...) {
        return false;
    }
}

Array ocgd_brep_reader::get_supported_extensions() const {
    Array extensions;
    extensions.append("brep");
    extensions.append("brp");
    return extensions;
}

Dictionary ocgd_brep_reader::analyze_shape(const Ref<ocgd_shape>& shape) const {
    Dictionary analysis;
    
    ERR_FAIL_NULL_V_MSG(shape.ptr(), analysis, "Shape reference is null");
    
    TopoDS_Shape occ_shape = shape->get_shape();
    ERR_FAIL_COND_V_MSG(occ_shape.IsNull(), analysis, "OpenCASCADE shape is null");
    
    analysis["valid"] = true;
    
    // Count different types of shapes
    int face_count = 0, edge_count = 0, vertex_count = 0, solid_count = 0;
    
    for (TopExp_Explorer exp(occ_shape, TopAbs_FACE); exp.More(); exp.Next()) face_count++;
    for (TopExp_Explorer exp(occ_shape, TopAbs_EDGE); exp.More(); exp.Next()) edge_count++;
    for (TopExp_Explorer exp(occ_shape, TopAbs_VERTEX); exp.More(); exp.Next()) vertex_count++;
    for (TopExp_Explorer exp(occ_shape, TopAbs_SOLID); exp.More(); exp.Next()) solid_count++;
    
    analysis["face_count"] = face_count;
    analysis["edge_count"] = edge_count;
    analysis["vertex_count"] = vertex_count;
    analysis["solid_count"] = solid_count;
    
    // Bounding box
    Bnd_Box bbox;
    BRepBndLib::Add(occ_shape, bbox);
    if (!bbox.IsVoid()) {
        double xmin, ymin, zmin, xmax, ymax, zmax;
        bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        
        Dictionary bounds;
        bounds["min_x"] = xmin;
        bounds["min_y"] = ymin;
        bounds["min_z"] = zmin;
        bounds["max_x"] = xmax;
        bounds["max_y"] = ymax;
        bounds["max_z"] = zmax;
        bounds["size_x"] = xmax - xmin;
        bounds["size_y"] = ymax - ymin;
        bounds["size_z"] = zmax - zmin;
        
        analysis["bounding_box"] = bounds;
    }
    
    return analysis;
}

int ocgd_brep_reader::get_shape_complexity(const Ref<ocgd_shape>& shape) const {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), 0, "Shape reference is null");
    
    TopoDS_Shape occ_shape = shape->get_shape();
    ERR_FAIL_COND_V_MSG(occ_shape.IsNull(), 0, "OpenCASCADE shape is null");
    
    int complexity = 0;
    
    for (TopExp_Explorer exp(occ_shape, TopAbs_FACE); exp.More(); exp.Next()) complexity += 10;
    for (TopExp_Explorer exp(occ_shape, TopAbs_EDGE); exp.More(); exp.Next()) complexity += 2;
    for (TopExp_Explorer exp(occ_shape, TopAbs_VERTEX); exp.More(); exp.Next()) complexity += 1;
    
    return complexity;
}

bool ocgd_brep_reader::is_brep_file(const String& file_path) const {
    String ext = file_path.get_extension().to_lower();
    return (ext == "brep" || ext == "brp");
}

String ocgd_brep_reader::detect_brep_version(const String& file_path) const {
    CharString path_utf8 = file_path.utf8();
    std::ifstream file(path_utf8.get_data());
    
    if (!file.is_open()) {
        return "unknown";
    }
    
    std::string first_line;
    std::getline(file, first_line);
    file.close();
    
    // BREP files typically start with version information
    if (first_line.find("DBRep_DrawableShape") != std::string::npos) {
        return "OpenCASCADE BREP";
    }
    
    return "generic";
}

String ocgd_brep_reader::get_type() const {
    return "ocgd_brep_reader";
}

bool ocgd_brep_reader::is_null() const {
    return false;
}

void ocgd_brep_reader::set_merge_vertices(bool enable) {
    merge_vertices = enable;
}

bool ocgd_brep_reader::get_merge_vertices() const {
    return merge_vertices;
}

void ocgd_brep_reader::set_fix_shapes(bool enable) {
    fix_shapes = enable;
}

bool ocgd_brep_reader::get_fix_shapes() const {
    return fix_shapes;
}

void ocgd_brep_reader::set_memory_limit(int limit_mb) {
    ERR_FAIL_COND_MSG(limit_mb < 0, "Memory limit cannot be negative");
    memory_limit = limit_mb;
}

int ocgd_brep_reader::get_memory_limit() const {
    return memory_limit;
}

Dictionary ocgd_brep_reader::get_load_statistics() const {
    Dictionary stats;
    stats["type"] = "BREP Load Statistics";
    stats["load_time"] = load_time;
    stats["memory_used"] = memory_used;
    stats["shapes_loaded"] = shapes_loaded;
    return stats;
}

double ocgd_brep_reader::get_load_time() const {
    return load_time;
}