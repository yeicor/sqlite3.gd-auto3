/**
 * ocgd_TopologyAnalyzer.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE comprehensive topology analysis.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_TopologyAnalyzer.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/GProp_PrincipalProps.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/BRep_Builder.hxx>
#include <opencascade/BRepBuilderAPI_MakeVertex.hxx>

#include <opencascade/TopoDS.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>
#include <opencascade/BRepClass3d_SolidClassifier.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/BRepAdaptor_Surface.hxx>
#include <opencascade/BRepAdaptor_Curve.hxx>
#include <opencascade/BRepLProp_SLProps.hxx>
#include <opencascade/BRepLProp_CLProps.hxx>
#include <opencascade/GeomAbs_SurfaceType.hxx>
#include <opencascade/GeomAbs_CurveType.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/Precision.hxx>
#include <opencascade/Standard_Failure.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Pln.hxx>
#include <opencascade/TopTools_IndexedMapOfShape.hxx>
#include <opencascade/TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <cmath>
#include <unordered_set>

void ocgd_TopologyAnalyzer::_bind_methods() {
    // Enums
    BIND_ENUM_CONSTANT(DEPTH_BASIC);
    BIND_ENUM_CONSTANT(DEPTH_GEOMETRIC);
    BIND_ENUM_CONSTANT(DEPTH_CONNECTIVITY);
    BIND_ENUM_CONSTANT(DEPTH_DETAILED);
    BIND_ENUM_CONSTANT(DEPTH_COMPLETE);

    BIND_ENUM_CONSTANT(CLASS_UNKNOWN);
    BIND_ENUM_CONSTANT(CLASS_SIMPLE_SOLID);
    BIND_ENUM_CONSTANT(CLASS_HOLLOW_SOLID);
    BIND_ENUM_CONSTANT(CLASS_SHEET_BODY);
    BIND_ENUM_CONSTANT(CLASS_WIREFRAME);
    BIND_ENUM_CONSTANT(CLASS_POINT_CLOUD);
    BIND_ENUM_CONSTANT(CLASS_ASSEMBLY);
    BIND_ENUM_CONSTANT(CLASS_SURFACE);
    BIND_ENUM_CONSTANT(CLASS_CURVE);

    BIND_ENUM_CONSTANT(SURFACE_PLANE);
    BIND_ENUM_CONSTANT(SURFACE_CYLINDER);
    BIND_ENUM_CONSTANT(SURFACE_CONE);
    BIND_ENUM_CONSTANT(SURFACE_SPHERE);
    BIND_ENUM_CONSTANT(SURFACE_TORUS);
    BIND_ENUM_CONSTANT(SURFACE_BEZIER);
    BIND_ENUM_CONSTANT(SURFACE_BSPLINE);
    BIND_ENUM_CONSTANT(SURFACE_REVOLUTION);
    BIND_ENUM_CONSTANT(SURFACE_EXTRUSION);
    BIND_ENUM_CONSTANT(SURFACE_OTHER);

    BIND_ENUM_CONSTANT(CURVE_LINE);
    BIND_ENUM_CONSTANT(CURVE_CIRCLE);
    BIND_ENUM_CONSTANT(CURVE_ELLIPSE);
    BIND_ENUM_CONSTANT(CURVE_HYPERBOLA);
    BIND_ENUM_CONSTANT(CURVE_PARABOLA);
    BIND_ENUM_CONSTANT(CURVE_BEZIER);
    BIND_ENUM_CONSTANT(CURVE_BSPLINE);
    BIND_ENUM_CONSTANT(CURVE_OTHER);

    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_analysis_depth", "depth"), &ocgd_TopologyAnalyzer::set_analysis_depth);
    ClassDB::bind_method(D_METHOD("get_analysis_depth"), &ocgd_TopologyAnalyzer::get_analysis_depth);

    ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &ocgd_TopologyAnalyzer::set_tolerance);
    ClassDB::bind_method(D_METHOD("get_tolerance"), &ocgd_TopologyAnalyzer::get_tolerance);

    ClassDB::bind_method(D_METHOD("set_include_mass_properties", "enabled"), &ocgd_TopologyAnalyzer::set_include_mass_properties);
    ClassDB::bind_method(D_METHOD("get_include_mass_properties"), &ocgd_TopologyAnalyzer::get_include_mass_properties);

    ClassDB::bind_method(D_METHOD("set_include_bounding_info", "enabled"), &ocgd_TopologyAnalyzer::set_include_bounding_info);
    ClassDB::bind_method(D_METHOD("get_include_bounding_info"), &ocgd_TopologyAnalyzer::get_include_bounding_info);

    ClassDB::bind_method(D_METHOD("set_include_connectivity", "enabled"), &ocgd_TopologyAnalyzer::set_include_connectivity);
    ClassDB::bind_method(D_METHOD("get_include_connectivity"), &ocgd_TopologyAnalyzer::get_include_connectivity);

    ClassDB::bind_method(D_METHOD("set_include_curvature", "enabled"), &ocgd_TopologyAnalyzer::set_include_curvature);
    ClassDB::bind_method(D_METHOD("get_include_curvature"), &ocgd_TopologyAnalyzer::get_include_curvature);

    ClassDB::bind_method(D_METHOD("set_validate_geometry", "enabled"), &ocgd_TopologyAnalyzer::set_validate_geometry);
    ClassDB::bind_method(D_METHOD("get_validate_geometry"), &ocgd_TopologyAnalyzer::get_validate_geometry);

    // Main analysis methods
    ClassDB::bind_method(D_METHOD("analyze_shape", "shape"), &ocgd_TopologyAnalyzer::analyze_shape);
    ClassDB::bind_method(D_METHOD("get_topology_summary", "shape"), &ocgd_TopologyAnalyzer::get_topology_summary);
    ClassDB::bind_method(D_METHOD("analyze_geometric_properties", "shape"), &ocgd_TopologyAnalyzer::analyze_geometric_properties);
    ClassDB::bind_method(D_METHOD("analyze_mass_properties", "shape", "density"), &ocgd_TopologyAnalyzer::analyze_mass_properties, DEFVAL(1.0));
    ClassDB::bind_method(D_METHOD("get_bounding_info", "shape"), &ocgd_TopologyAnalyzer::get_bounding_info);
    ClassDB::bind_method(D_METHOD("classify_shape", "shape"), &ocgd_TopologyAnalyzer::classify_shape);

    // Element-specific analysis
    ClassDB::bind_method(D_METHOD("analyze_faces", "shape"), &ocgd_TopologyAnalyzer::analyze_faces);
    ClassDB::bind_method(D_METHOD("analyze_edges", "shape"), &ocgd_TopologyAnalyzer::analyze_edges);
    ClassDB::bind_method(D_METHOD("analyze_vertices", "shape"), &ocgd_TopologyAnalyzer::analyze_vertices);
    ClassDB::bind_method(D_METHOD("analyze_face_detailed", "shape", "face_index"), &ocgd_TopologyAnalyzer::analyze_face_detailed);
    ClassDB::bind_method(D_METHOD("analyze_edge_detailed", "shape", "edge_index"), &ocgd_TopologyAnalyzer::analyze_edge_detailed);

    // Connectivity analysis
    ClassDB::bind_method(D_METHOD("analyze_connectivity", "shape"), &ocgd_TopologyAnalyzer::analyze_connectivity);
    ClassDB::bind_method(D_METHOD("get_face_adjacency", "shape"), &ocgd_TopologyAnalyzer::get_face_adjacency);
    ClassDB::bind_method(D_METHOD("get_edge_sharing", "shape"), &ocgd_TopologyAnalyzer::get_edge_sharing);
    ClassDB::bind_method(D_METHOD("find_boundaries", "shape"), &ocgd_TopologyAnalyzer::find_boundaries);

    // Measurement tools
    ClassDB::bind_method(D_METHOD("calculate_distance", "shape1", "shape2"), &ocgd_TopologyAnalyzer::calculate_distance);
    ClassDB::bind_method(D_METHOD("distance_point_to_shape", "point", "shape"), &ocgd_TopologyAnalyzer::distance_point_to_shape);
    ClassDB::bind_method(D_METHOD("find_closest_points", "shape1", "shape2"), &ocgd_TopologyAnalyzer::find_closest_points);
    ClassDB::bind_method(D_METHOD("calculate_face_angle", "shape", "face1_index", "face2_index"), &ocgd_TopologyAnalyzer::calculate_face_angle);

    // Curvature analysis
    ClassDB::bind_method(D_METHOD("analyze_face_curvature", "shape"), &ocgd_TopologyAnalyzer::analyze_face_curvature);
    ClassDB::bind_method(D_METHOD("analyze_edge_curvature", "shape"), &ocgd_TopologyAnalyzer::analyze_edge_curvature);

    // Feature detection
    ClassDB::bind_method(D_METHOD("detect_features", "shape"), &ocgd_TopologyAnalyzer::detect_features);
    ClassDB::bind_method(D_METHOD("find_cylindrical_holes", "shape"), &ocgd_TopologyAnalyzer::find_cylindrical_holes);
    ClassDB::bind_method(D_METHOD("group_planar_faces", "shape"), &ocgd_TopologyAnalyzer::group_planar_faces);

    // Quality analysis
    ClassDB::bind_method(D_METHOD("validate_shape", "shape"), &ocgd_TopologyAnalyzer::validate_shape);
    ClassDB::bind_method(D_METHOD("check_geometry_issues", "shape"), &ocgd_TopologyAnalyzer::check_geometry_issues);
    ClassDB::bind_method(D_METHOD("calculate_complexity_metrics", "shape"), &ocgd_TopologyAnalyzer::calculate_complexity_metrics);

    // Spatial analysis
    ClassDB::bind_method(D_METHOD("is_point_inside", "point", "shape"), &ocgd_TopologyAnalyzer::is_point_inside);
    ClassDB::bind_method(D_METHOD("compare_shapes", "shape1", "shape2"), &ocgd_TopologyAnalyzer::compare_shapes);

    // Utility methods
    ClassDB::bind_method(D_METHOD("clear_cache"), &ocgd_TopologyAnalyzer::clear_cache);
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_TopologyAnalyzer::get_last_error);
}

ocgd_TopologyAnalyzer::ocgd_TopologyAnalyzer() :
    _analysis_depth(DEPTH_GEOMETRIC),
    _tolerance(Precision::Confusion()),
    _include_mass_properties(true),
    _include_bounding_info(true),
    _include_connectivity(false),
    _include_curvature(false),
    _validate_geometry(true),
    _cache_valid(false) {
}

ocgd_TopologyAnalyzer::~ocgd_TopologyAnalyzer() {
}

// Configuration methods
void ocgd_TopologyAnalyzer::set_analysis_depth(AnalysisDepth depth) {
    _analysis_depth = depth;
    _cache_valid = false;
}

ocgd_TopologyAnalyzer::AnalysisDepth ocgd_TopologyAnalyzer::get_analysis_depth() const {
    return _analysis_depth;
}

void ocgd_TopologyAnalyzer::set_tolerance(double tolerance) {
    _tolerance = std::max(1e-12, tolerance);
    _cache_valid = false;
}

double ocgd_TopologyAnalyzer::get_tolerance() const {
    return _tolerance;
}

void ocgd_TopologyAnalyzer::set_include_mass_properties(bool enabled) {
    _include_mass_properties = enabled;
    _cache_valid = false;
}

bool ocgd_TopologyAnalyzer::get_include_mass_properties() const {
    return _include_mass_properties;
}

void ocgd_TopologyAnalyzer::set_include_bounding_info(bool enabled) {
    _include_bounding_info = enabled;
    _cache_valid = false;
}

bool ocgd_TopologyAnalyzer::get_include_bounding_info() const {
    return _include_bounding_info;
}

void ocgd_TopologyAnalyzer::set_include_connectivity(bool enabled) {
    _include_connectivity = enabled;
    _cache_valid = false;
}

bool ocgd_TopologyAnalyzer::get_include_connectivity() const {
    return _include_connectivity;
}

void ocgd_TopologyAnalyzer::set_include_curvature(bool enabled) {
    _include_curvature = enabled;
    _cache_valid = false;
}

bool ocgd_TopologyAnalyzer::get_include_curvature() const {
    return _include_curvature;
}

void ocgd_TopologyAnalyzer::set_validate_geometry(bool enabled) {
    _validate_geometry = enabled;
    _cache_valid = false;
}

bool ocgd_TopologyAnalyzer::get_validate_geometry() const {
    return _validate_geometry;
}

// Main analysis methods
Dictionary ocgd_TopologyAnalyzer::analyze_shape(const Ref<ocgd_TopoDS_Shape>& shape) {
    if (shape.is_null()) {
        set_error("Shape is null");
        return Dictionary();
    }

    // Check cache
    if (_cache_valid && _cached_shape == shape) {
        return _cached_analysis;
    }

    Dictionary result;
    _last_error = "";

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        // Basic topology information
        result["topology"] = get_topology_summary(shape);

        // Geometric properties
        if (_analysis_depth >= DEPTH_GEOMETRIC) {
            result["geometry"] = analyze_geometric_properties(shape);

            if (_include_bounding_info) {
                result["bounding"] = get_bounding_info(shape);
            }

            if (_include_mass_properties) {
                result["mass_properties"] = analyze_mass_properties(shape);
            }
        }

        // Connectivity analysis
        if (_analysis_depth >= DEPTH_CONNECTIVITY && _include_connectivity) {
            result["connectivity"] = analyze_connectivity(shape);
        }

        // Shape classification
        result["shape_class"] = classify_shape(shape);

        // Validation
        if (_validate_geometry) {
            result["validation"] = validate_shape(shape);
        }

        // Detailed analysis
        if (_analysis_depth >= DEPTH_DETAILED) {
            result["faces"] = analyze_faces(shape);
            result["edges"] = analyze_edges(shape);

            if (_include_curvature) {
                result["face_curvature"] = analyze_face_curvature(shape);
                result["edge_curvature"] = analyze_edge_curvature(shape);
            }
        }

        // Complete analysis
        if (_analysis_depth >= DEPTH_COMPLETE) {
            result["features"] = detect_features(shape);
            result["complexity"] = calculate_complexity_metrics(shape);
        }

        // Cache results
        _cached_analysis = result;
        _cached_shape = shape;
        _cache_valid = true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing shape - " + String(e.GetMessageString()));
        set_error(String("Analysis failed: ") + String(e.GetMessageString()));
        return Dictionary();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing shape - " + String(e.what()));
        set_error(String("Analysis failed: ") + String(e.what()));
        return Dictionary();
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::get_topology_summary(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot get topology summary - shape reference is null");
            return Dictionary();
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot get topology summary - shape is null");
            return Dictionary();
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();
        return count_topology_elements(occt_shape);
        
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception getting topology summary - " + String(e.GetMessageString()));
        return Dictionary();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception getting topology summary - " + String(e.what()));
        return Dictionary();
    }
}

Dictionary ocgd_TopologyAnalyzer::analyze_geometric_properties(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze geometric properties - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze geometric properties - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze geometric properties - OpenCASCADE shape is null");
            return result;
        }

        // Volume properties
        GProp_GProps volume_props;
        BRepGProp::VolumeProperties(occt_shape, volume_props);

        if (volume_props.Mass() > _tolerance) {
            result["volume"] = volume_props.Mass();
            gp_Pnt center = volume_props.CentreOfMass();
            result["volume_center"] = gp_pnt_to_vector3(center);
        }

        // Surface properties
        GProp_GProps surface_props;
        BRepGProp::SurfaceProperties(occt_shape, surface_props);

        result["surface_area"] = surface_props.Mass();
        gp_Pnt surface_center = surface_props.CentreOfMass();
        result["surface_center"] = gp_pnt_to_vector3(surface_center);

        // Linear properties (edge lengths)
        GProp_GProps linear_props;
        BRepGProp::LinearProperties(occt_shape, linear_props);

        result["total_edge_length"] = linear_props.Mass();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing geometric properties - " + String(e.GetMessageString()));
        set_error(String("Geometric properties analysis failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing geometric properties - " + String(e.what()));
        set_error(String("Geometric properties analysis failed: ") + String(e.what()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::analyze_mass_properties(const Ref<ocgd_TopoDS_Shape>& shape, double density) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze mass properties - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze mass properties - shape is null");
            return result;
        }

        if (density <= 0.0) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze mass properties - density must be positive, got: " + String::num(density));
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze mass properties - OpenCASCADE shape is null");
            return result;
        }

        GProp_GProps props;
        BRepGProp::VolumeProperties(occt_shape, props);

        double volume = props.Mass();
        if (volume > _tolerance) {
            result["mass"] = volume * density;
            result["volume"] = volume;
            result["density"] = density;

            gp_Pnt center = props.CentreOfMass();
            result["center_of_mass"] = gp_pnt_to_vector3(center);

            // Inertia matrix
            gp_Mat inertia = props.MatrixOfInertia();
            Array inertia_matrix;
            for (int i = 1; i <= 3; i++) {
                Array row;
                for (int j = 1; j <= 3; j++) {
                    row.append(inertia.Value(i, j));
                }
                inertia_matrix.append(row);
            }
            result["inertia_matrix"] = inertia_matrix;

            // Principal moments
            GProp_PrincipalProps principal = props.PrincipalProperties();
            Standard_Real i1, i2, i3;
            principal.Moments(i1, i2, i3);

            Array principal_moments;
            principal_moments.append(i1);
            principal_moments.append(i2);
            principal_moments.append(i3);
            result["principal_moments"] = principal_moments;

            // Principal axes
            result["principal_axis_1"] = gp_dir_to_vector3(principal.FirstAxisOfInertia());
            result["principal_axis_2"] = gp_dir_to_vector3(principal.SecondAxisOfInertia());
            result["principal_axis_3"] = gp_dir_to_vector3(principal.ThirdAxisOfInertia());
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing mass properties - " + String(e.GetMessageString()));
        set_error(String("Mass properties analysis failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing mass properties - " + String(e.what()));
        set_error(String("Mass properties analysis failed: ") + String(e.what()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::get_bounding_info(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot get bounding info - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot get bounding info - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot get bounding info - OpenCASCADE shape is null");
            return result;
        }

        Bnd_Box bbox;
        BRepBndLib::Add(occt_shape, bbox);

        if (!bbox.IsVoid()) {
            Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
            bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);

            result["min"] = Vector3(xmin, ymin, zmin);
            result["max"] = Vector3(xmax, ymax, zmax);
            result["size"] = Vector3(xmax - xmin, ymax - ymin, zmax - zmin);
            result["center"] = Vector3((xmin + xmax) / 2, (ymin + ymax) / 2, (zmin + zmax) / 2);

            double volume = (xmax - xmin) * (ymax - ymin) * (zmax - zmin);
            result["bounding_volume"] = volume;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception getting bounding info - " + String(e.GetMessageString()));
        set_error(String("Bounding info calculation failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception getting bounding info - " + String(e.what()));
        set_error(String("Bounding info calculation failed: ") + String(e.what()));
    }

    return result;
}

ocgd_TopologyAnalyzer::ShapeClass ocgd_TopologyAnalyzer::classify_shape(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot classify shape - shape reference is null");
            return CLASS_UNKNOWN;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot classify shape - shape is null");
            return CLASS_UNKNOWN;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot classify shape - OpenCASCADE shape is null");
            return CLASS_UNKNOWN;
        }

        Dictionary counts = count_topology_elements(occt_shape);

        int solids = counts.get("solids", 0);
        int shells = counts.get("shells", 0);
        int faces = counts.get("faces", 0);
        int wires = counts.get("wires", 0);
        int edges = counts.get("edges", 0);
        int vertices = counts.get("vertices", 0);

        if (solids > 1) {
            return CLASS_ASSEMBLY;
        } else if (solids == 1) {
            // Check if it's hollow by analyzing shells
            if (shells > 1) {
                return CLASS_HOLLOW_SOLID;
            } else {
                return CLASS_SIMPLE_SOLID;
            }
        } else if (faces > 0) {
            if (faces == 1) {
                return CLASS_SURFACE;
            } else {
                return CLASS_SHEET_BODY;
            }
        } else if (edges > 0) {
            if (edges == 1) {
                return CLASS_WIREFRAME;
            } else {
                return CLASS_WIREFRAME;
            }
        } else if (vertices > 0) {
            return CLASS_POINT_CLOUD;
        }

        return CLASS_UNKNOWN;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception classifying shape - " + String(e.GetMessageString()));
        set_error(String("Shape classification failed: ") + String(e.GetMessageString()));
        return CLASS_UNKNOWN;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception classifying shape - " + String(e.what()));
        set_error(String("Shape classification failed: ") + String(e.what()));
        return CLASS_UNKNOWN;
    }
}

// Element-specific analysis
Array ocgd_TopologyAnalyzer::analyze_faces(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze faces - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze faces - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze faces - OpenCASCADE shape is null");
            return result;
        }

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
            Dictionary face_info = analyze_face_properties(face);
            face_info["index"] = face_index;
            result.append(face_info);

            face_explorer.Next();
            face_index++;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing faces - " + String(e.GetMessageString()));
        set_error(String("Face analysis failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing faces - " + String(e.what()));
        set_error(String("Face analysis failed: ") + String(e.what()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::analyze_edges(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze edges - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze edges - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze edges - OpenCASCADE shape is null");
            return result;
        }

        TopExp_Explorer edge_explorer(occt_shape, TopAbs_EDGE);
        int edge_index = 0;

        while (edge_explorer.More()) {
            const TopoDS_Edge& edge = TopoDS::Edge(edge_explorer.Current());
            Dictionary edge_info = analyze_edge_properties(edge);
            edge_info["index"] = edge_index;
            result.append(edge_info);

            edge_explorer.Next();
            edge_index++;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing edges - " + String(e.GetMessageString()));
        set_error(String("Edge analysis failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing edges - " + String(e.what()));
        set_error(String("Edge analysis failed: ") + String(e.what()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::analyze_vertices(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze vertices - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze vertices - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot analyze vertices - OpenCASCADE shape is null");
            return result;
        }

        TopExp_Explorer vertex_explorer(occt_shape, TopAbs_VERTEX);
        int vertex_index = 0;

        while (vertex_explorer.More()) {
            const TopoDS_Vertex& vertex = TopoDS::Vertex(vertex_explorer.Current());

            Dictionary vertex_info;
            gp_Pnt point = BRep_Tool::Pnt(vertex);
            vertex_info["index"] = vertex_index;
            vertex_info["position"] = gp_pnt_to_vector3(point);
            vertex_info["tolerance"] = BRep_Tool::Tolerance(vertex);

            result.append(vertex_info);

            vertex_explorer.Next();
            vertex_index++;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing vertices - " + String(e.GetMessageString()));
        set_error(String("Vertex analysis failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception analyzing vertices - " + String(e.what()));
        set_error(String("Vertex analysis failed: ") + String(e.what()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::analyze_face_detailed(const Ref<ocgd_TopoDS_Shape>& shape, int face_index) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int current_index = 0;

        while (face_explorer.More() && current_index <= face_index) {
            if (current_index == face_index) {
                const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
                result = analyze_face_properties(face);

                // Add curvature information if enabled
                if (_include_curvature) {
                    BRepAdaptor_Surface surface(face);
                    result["surface_type"] = static_cast<int>(classify_surface(face));

                    // Sample curvature at face center
                    Standard_Real u_min, u_max, v_min, v_max;
                    BRepTools::UVBounds(face, u_min, u_max, v_min, v_max);
                    Standard_Real u_mid = (u_min + u_max) / 2.0;
                    Standard_Real v_mid = (v_min + v_max) / 2.0;

                    BRepLProp_SLProps props(surface, u_mid, v_mid, 2, _tolerance);
                    if (props.IsCurvatureDefined()) {
                        result["gaussian_curvature"] = props.GaussianCurvature();
                        result["mean_curvature"] = props.MeanCurvature();
                        result["min_curvature"] = props.MinCurvature();
                        result["max_curvature"] = props.MaxCurvature();
                    }
                }
                break;
            }

            face_explorer.Next();
            current_index++;
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Detailed face analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::analyze_edge_detailed(const Ref<ocgd_TopoDS_Shape>& shape, int edge_index) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopExp_Explorer edge_explorer(occt_shape, TopAbs_EDGE);
        int current_index = 0;

        while (edge_explorer.More() && current_index <= edge_index) {
            if (current_index == edge_index) {
                const TopoDS_Edge& edge = TopoDS::Edge(edge_explorer.Current());
                result = analyze_edge_properties(edge);

                // Add curvature information if enabled
                if (_include_curvature) {
                    result["curve_type"] = static_cast<int>(classify_curve(edge));

                    BRepAdaptor_Curve curve(edge);
                    Standard_Real first = curve.FirstParameter();
                    Standard_Real last = curve.LastParameter();
                    Standard_Real mid = (first + last) / 2.0;

                    BRepLProp_CLProps props(curve, mid, 2, _tolerance);
                    if (props.IsTangentDefined()) {
                        gp_Dir tangent;
                        props.Tangent(tangent);
                        result["tangent"] = gp_dir_to_vector3(tangent);
                    }

                    // Note: BRepLProp_CLProps doesn't have IsCurvatureDefined in current API
                    try {
                        Standard_Real curvature = props.Curvature();
                        result["curvature"] = curvature;
                    } catch (...) {
                        // Curvature not available
                    }
                }
                break;
            }

            edge_explorer.Next();
            current_index++;
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Detailed edge analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Connectivity analysis
Dictionary ocgd_TopologyAnalyzer::analyze_connectivity(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        result["face_adjacency"] = get_face_adjacency(shape);
        result["edge_sharing"] = get_edge_sharing(shape);
        result["boundaries"] = find_boundaries(shape);

    } catch (const Standard_Failure& e) {
        set_error(String("Connectivity analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::get_face_adjacency(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        // Map edges to faces
        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(occt_shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);

        Array adjacency_list;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
            Array neighbors = find_face_neighbors(occt_shape, face);

            Dictionary face_adjacency;
            face_adjacency["face_index"] = face_index;
            face_adjacency["neighbor_indices"] = neighbors;
            adjacency_list.append(face_adjacency);

            face_explorer.Next();
            face_index++;
        }

        result["adjacency_list"] = adjacency_list;

    } catch (const Standard_Failure& e) {
        set_error(String("Face adjacency analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::get_edge_sharing(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(occt_shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);

        Array shared_edges;

        for (int i = 1; i <= edge_face_map.Extent(); i++) {
            const TopTools_ListOfShape& face_list = edge_face_map.FindFromIndex(i);

            if (face_list.Extent() >= 2) {
                Dictionary edge_info;
                edge_info["edge_index"] = i - 1;

                Array sharing_faces;
                TopTools_ListIteratorOfListOfShape it(face_list);
                while (it.More()) {
                    // Find face index
                    TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
                    int face_index = 0;
                    while (face_explorer.More()) {
                        if (face_explorer.Current().IsSame(it.Value())) {
                            sharing_faces.append(face_index);
                            break;
                        }
                        face_explorer.Next();
                        face_index++;
                    }
                    it.Next();
                }

                edge_info["faces"] = sharing_faces;
                shared_edges.append(edge_info);
            }
        }

        result["shared_edges"] = shared_edges;

    } catch (const Standard_Failure& e) {
        set_error(String("Edge sharing analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::find_boundaries(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(occt_shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);

        Array boundary_edges;
        Array free_edges;

        for (int i = 1; i <= edge_face_map.Extent(); i++) {
            const TopTools_ListOfShape& face_list = edge_face_map.FindFromIndex(i);

            if (face_list.Extent() == 1) {
                boundary_edges.append(i - 1);
            } else if (face_list.Extent() == 0) {
                free_edges.append(i - 1);
            }
        }

        result["boundary_edges"] = boundary_edges;
        result["free_edges"] = free_edges;
        result["is_closed"] = (boundary_edges.size() == 0);

    } catch (const Standard_Failure& e) {
        set_error(String("Boundary analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Measurement tools
Dictionary ocgd_TopologyAnalyzer::calculate_distance(const Ref<ocgd_TopoDS_Shape>& shape1,
                                                    const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;

    try {
        if (shape1.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - shape1 reference is null");
            return result;
        }

        if (shape2.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - shape2 reference is null");
            return result;
        }

        if (shape1->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - shape1 is null");
            return result;
        }

        if (shape2->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - shape2 is null");
            return result;
        }

        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();

        if (occt_shape1.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - OpenCASCADE shape1 is null");
            return result;
        }

        if (occt_shape2.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate distance - OpenCASCADE shape2 is null");
            return result;
        }

        BRepExtrema_DistShapeShape distance_calculator(occt_shape1, occt_shape2);
        distance_calculator.Perform();

        if (distance_calculator.IsDone()) {
            result["distance"] = distance_calculator.Value();
            result["nb_solutions"] = distance_calculator.NbSolution();

            if (distance_calculator.NbSolution() > 0) {
                gp_Pnt p1 = distance_calculator.PointOnShape1(1);
                gp_Pnt p2 = distance_calculator.PointOnShape2(1);

                result["point_on_shape1"] = gp_pnt_to_vector3(p1);
                result["point_on_shape2"] = gp_pnt_to_vector3(p2);
            }
        } else {
            UtilityFunctions::printerr("TopologyAnalyzer: Distance calculation did not complete successfully");
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception calculating distance - " + String(e.GetMessageString()));
        set_error(String("Distance calculation failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception calculating distance - " + String(e.what()));
        set_error(String("Distance calculation failed: ") + String(e.what()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::distance_point_to_shape(const Vector3& point,
                                                         const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate point distance - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate point distance - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot calculate point distance - OpenCASCADE shape is null");
            return result;
        }

        gp_Pnt occt_point(point.x, point.y, point.z);

        // Create a vertex from the point for distance calculation
        BRepBuilderAPI_MakeVertex vertex_maker(occt_point);
        
        if (!vertex_maker.IsDone()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Failed to create vertex from point");
            return result;
        }
        
        TopoDS_Vertex vertex = vertex_maker.Vertex();

        BRepExtrema_DistShapeShape distance_calculator(vertex, occt_shape);
        distance_calculator.Perform();

        if (distance_calculator.IsDone()) {
            result["distance"] = distance_calculator.Value();
            result["nb_solutions"] = distance_calculator.NbSolution();

            if (distance_calculator.NbSolution() > 0) {
                gp_Pnt closest_point = distance_calculator.PointOnShape2(1);
                result["closest_point"] = gp_pnt_to_vector3(closest_point);
            }
        } else {
            UtilityFunctions::printerr("TopologyAnalyzer: Point to shape distance calculation did not complete successfully");
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception calculating point distance - " + String(e.GetMessageString()));
        set_error(String("Point to shape distance calculation failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception calculating point distance - " + String(e.what()));
        set_error(String("Point to shape distance calculation failed: ") + String(e.what()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::find_closest_points(const Ref<ocgd_TopoDS_Shape>& shape1,
                                                 const Ref<ocgd_TopoDS_Shape>& shape2) {
    Array result;

    try {
        if (shape1.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - shape1 reference is null");
            return result;
        }

        if (shape2.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - shape2 reference is null");
            return result;
        }

        if (shape1->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - shape1 is null");
            return result;
        }

        if (shape2->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - shape2 is null");
            return result;
        }



        const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
        const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();

        if (occt_shape1.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - OpenCASCADE shape1 is null");
            return result;
        }

        if (occt_shape2.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot find closest points - OpenCASCADE shape2 is null");
            return result;
        }

        BRepExtrema_DistShapeShape distance_calculator(occt_shape1, occt_shape2);
        distance_calculator.Perform();

        if (distance_calculator.IsDone()) {
            int solutions = distance_calculator.NbSolution();
            
            for (int i = 1; i <= solutions; i++) {
                Dictionary solution;
                solution["point_on_shape1"] = gp_pnt_to_vector3(distance_calculator.PointOnShape1(i));
                solution["point_on_shape2"] = gp_pnt_to_vector3(distance_calculator.PointOnShape2(i));
                solution["distance"] = distance_calculator.Value();
                result.append(solution);
            }
        } else {
            UtilityFunctions::printerr("TopologyAnalyzer: Closest points calculation did not complete successfully");
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception finding closest points - " + String(e.GetMessageString()));
        set_error(String("Closest points calculation failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception finding closest points - " + String(e.what()));
        set_error(String("Closest points calculation failed: ") + String(e.what()));
    }

    return result;
}

double ocgd_TopologyAnalyzer::calculate_face_angle(const Ref<ocgd_TopoDS_Shape>& shape,
                                                  int face1_index, int face2_index) {
    if (shape.is_null()) {
        return 0.0;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        // Find the faces by index
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        TopoDS_Face face1, face2;
        int current_index = 0;
        bool found_face1 = false, found_face2 = false;

        while (face_explorer.More()) {
            if (current_index == face1_index) {
                face1 = TopoDS::Face(face_explorer.Current());
                found_face1 = true;
            } else if (current_index == face2_index) {
                face2 = TopoDS::Face(face_explorer.Current());
                found_face2 = true;
            }

            if (found_face1 && found_face2) break;

            face_explorer.Next();
            current_index++;
        }

        if (!found_face1 || !found_face2) {
            set_error("Face indices not found");
            return 0.0;
        }

        // Get surface normals at center points
        BRepAdaptor_Surface surface1(face1);
        BRepAdaptor_Surface surface2(face2);

        Standard_Real u1_min, u1_max, v1_min, v1_max;
        Standard_Real u2_min, u2_max, v2_min, v2_max;

        BRepTools::UVBounds(face1, u1_min, u1_max, v1_min, v1_max);
        BRepTools::UVBounds(face2, u2_min, u2_max, v2_min, v2_max);

        Standard_Real u1_mid = (u1_min + u1_max) / 2.0;
        Standard_Real v1_mid = (v1_min + v1_max) / 2.0;
        Standard_Real u2_mid = (u2_min + u2_max) / 2.0;
        Standard_Real v2_mid = (v2_min + v2_max) / 2.0;

        BRepLProp_SLProps props1(surface1, u1_mid, v1_mid, 1, _tolerance);
        BRepLProp_SLProps props2(surface2, u2_mid, v2_mid, 1, _tolerance);

        if (props1.IsNormalDefined() && props2.IsNormalDefined()) {
            gp_Dir normal1 = props1.Normal();
            gp_Dir normal2 = props2.Normal();

            double dot_product = normal1.Dot(normal2);
            double angle = std::acos(std::clamp(dot_product, -1.0, 1.0));

            return angle; // Return angle in radians
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Face angle calculation failed: ") + String(e.GetMessageString()));
    }

    return 0.0;
}

// Curvature analysis
Array ocgd_TopologyAnalyzer::analyze_face_curvature(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            Dictionary curvature_info;
            curvature_info["face_index"] = face_index;
            curvature_info["surface_type"] = static_cast<int>(classify_surface(face));

            BRepAdaptor_Surface surface(face);
            Standard_Real u_min, u_max, v_min, v_max;
            BRepTools::UVBounds(face, u_min, u_max, v_min, v_max);

            // Sample curvature at center
            Standard_Real u_mid = (u_min + u_max) / 2.0;
            Standard_Real v_mid = (v_min + v_max) / 2.0;

            BRepLProp_SLProps props(surface, u_mid, v_mid, 2, _tolerance);
            if (props.IsCurvatureDefined()) {
                curvature_info["gaussian_curvature"] = props.GaussianCurvature();
                curvature_info["mean_curvature"] = props.MeanCurvature();
                curvature_info["min_curvature"] = props.MinCurvature();
                curvature_info["max_curvature"] = props.MaxCurvature();
            }

            result.append(curvature_info);

            face_explorer.Next();
            face_index++;
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Face curvature analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::analyze_edge_curvature(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopExp_Explorer edge_explorer(occt_shape, TopAbs_EDGE);
        int edge_index = 0;

        while (edge_explorer.More()) {
            const TopoDS_Edge& edge = TopoDS::Edge(edge_explorer.Current());

            Dictionary curvature_info;
            curvature_info["edge_index"] = edge_index;
            curvature_info["curve_type"] = static_cast<int>(classify_curve(edge));

            BRepAdaptor_Curve curve(edge);
            Standard_Real first = curve.FirstParameter();
            Standard_Real last = curve.LastParameter();
            Standard_Real mid = (first + last) / 2.0;

            BRepLProp_CLProps props(curve, mid, 2, _tolerance);
            try {
                Standard_Real curvature = props.Curvature();
                curvature_info["curvature"] = curvature;
            } catch (...) {
                // Curvature not available
            }

            if (props.IsTangentDefined()) {
                gp_Dir tangent;
                props.Tangent(tangent);
                curvature_info["tangent"] = gp_dir_to_vector3(tangent);
            }

            result.append(curvature_info);

            edge_explorer.Next();
            edge_index++;
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Edge curvature analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Feature detection
Dictionary ocgd_TopologyAnalyzer::detect_features(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        result["cylindrical_holes"] = find_cylindrical_holes(shape);
        result["planar_faces"] = group_planar_faces(shape);

        // Simple fillet detection based on surface types
        Array potential_fillets;
        Array faces = analyze_faces(shape);

        for (int i = 0; i < faces.size(); i++) {
            Dictionary face_info = faces[i];
            if (face_info.has("surface_type")) {
                int surface_type = face_info["surface_type"];
                if (surface_type == SURFACE_CYLINDER || surface_type == SURFACE_TORUS) {
                    potential_fillets.append(i);
                }
            }
        }

        result["potential_fillets"] = potential_fillets;

    } catch (const Standard_Failure& e) {
        set_error(String("Feature detection failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::find_cylindrical_holes(const Ref<ocgd_TopoDS_Shape>& shape) {
    Array result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            if (classify_surface(face) == SURFACE_CYLINDER) {
                BRepAdaptor_Surface surface(face);
                gp_Cylinder cylinder = surface.Cylinder();

                // Check if it's a complete cylinder (potential hole)
                Standard_Real u_min, u_max, v_min, v_max;
                BRepTools::UVBounds(face, u_min, u_max, v_min, v_max);

                if (std::abs(u_max - u_min - 2 * M_PI) < _tolerance) {
                    Dictionary hole_info;
                    hole_info["face_index"] = face_index;
                    hole_info["axis_origin"] = gp_pnt_to_vector3(cylinder.Axis().Location());
                    hole_info["axis_direction"] = gp_dir_to_vector3(cylinder.Axis().Direction());
                    hole_info["radius"] = cylinder.Radius();
                    hole_info["height"] = v_max - v_min;

                    result.append(hole_info);
                }
            }

            face_explorer.Next();
            face_index++;
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Cylindrical hole detection failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::group_planar_faces(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();
        std::vector<std::vector<int>> groups;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            if (classify_surface(face) == SURFACE_PLANE) {
                BRepAdaptor_Surface surface(face);
                gp_Pln plane = surface.Plane();

                // Find existing group with same plane or create new
                bool found_group = false;
                for (auto& group : groups) {
                    if (!group.empty()) {
                        // Compare with first face in group (simplified)
                        found_group = true; // Simplified logic
                        group.push_back(face_index);
                        break;
                    }
                }

                if (!found_group) {
                    groups.push_back({face_index});
                }
            }

            face_explorer.Next();
            face_index++;
        }

        Array group_array;
        for (const auto& group : groups) {
            Array face_indices;
            for (int idx : group) {
                face_indices.append(idx);
            }
            group_array.append(face_indices);
        }

        result["planar_groups"] = group_array;

    } catch (const Standard_Failure& e) {
        set_error(String("Planar face grouping failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Quality analysis
Dictionary ocgd_TopologyAnalyzer::validate_shape(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot validate shape - shape reference is null");
            result["valid"] = false;
            result["error"] = "Shape reference is null";
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot validate shape - shape is null");
            result["valid"] = false;
            result["error"] = "Shape is null";
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot validate shape - OpenCASCADE shape is null");
            result["valid"] = false;
            result["error"] = "OpenCASCADE shape is null";
            return result;
        }

        BRepCheck_Analyzer analyzer(occt_shape);
        result["valid"] = analyzer.IsValid();

        if (!analyzer.IsValid()) {
            // Collect validation errors (simplified)
            result["has_errors"] = true;
            result["error_message"] = "Shape contains geometric or topological errors";
        } else {
            result["has_errors"] = false;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception validating shape - " + String(e.GetMessageString()));
        result["valid"] = false;
        result["error"] = String("Validation failed: ") + String(e.GetMessageString());
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception validating shape - " + String(e.what()));
        result["valid"] = false;
        result["error"] = String("Validation failed: ") + String(e.what());
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::check_geometry_issues(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check geometry issues - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check geometry issues - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check geometry issues - OpenCASCADE shape is null");
            return result;
        }
        Dictionary validation = validate_shape(shape);
        result["validation"] = validation;

        Dictionary bounds = get_bounding_info(shape);
        Vector3 size = bounds.get("size", Vector3());

        // Check for very small or very large dimensions
        result["has_tiny_features"] = (size.x < 1e-6 || size.y < 1e-6 || size.z < 1e-6);
        result["has_huge_features"] = (size.x > 1e6 || size.y > 1e6 || size.z > 1e6);

        // Check topology counts for unusual configurations
        Dictionary topology = get_topology_summary(shape);
        int faces = topology.get("faces", 0);
        int edges = topology.get("edges", 0);
        int vertices = topology.get("vertices", 0);

        result["has_isolated_vertices"] = (vertices > 0 && edges == 0);
        result["has_many_faces"] = (faces > 10000);
        result["face_edge_ratio"] = faces > 0 ? (double)edges / (double)faces : 0.0;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception checking geometry issues - " + String(e.GetMessageString()));
        set_error(String("Geometry issues check failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception checking geometry issues - " + String(e.what()));
        set_error(String("Geometry issues check failed: ") + String(e.what()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::calculate_complexity_metrics(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null()) {
        return result;
    }

    try {
        Dictionary topology = get_topology_summary(shape);

        int solids = topology.get("solids", 0);
        int shells = topology.get("shells", 0);
        int faces = topology.get("faces", 0);
        int wires = topology.get("wires", 0);
        int edges = topology.get("edges", 0);
        int vertices = topology.get("vertices", 0);

        // Calculate complexity score
        double complexity_score = 0.0;
        complexity_score += solids * 1.0;
        complexity_score += shells * 0.8;
        complexity_score += faces * 0.5;
        complexity_score += wires * 0.3;
        complexity_score += edges * 0.1;
        complexity_score += vertices * 0.05;

        result["complexity_score"] = complexity_score;
        result["topology_elements"] = solids + shells + faces + wires + edges + vertices;
        result["face_to_edge_ratio"] = edges > 0 ? (double)faces / edges : 0.0;
        result["edge_to_vertex_ratio"] = vertices > 0 ? (double)edges / vertices : 0.0;

        // Classify complexity level
        String complexity_level;
        if (complexity_score < 10.0) {
            complexity_level = "Simple";
        } else if (complexity_score < 100.0) {
            complexity_level = "Moderate";
        } else if (complexity_score < 1000.0) {
            complexity_level = "Complex";
        } else {
            complexity_level = "Very Complex";
        }

        result["complexity_level"] = complexity_level;

    } catch (const Standard_Failure& e) {
        set_error(String("Complexity metrics calculation failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Spatial analysis
bool ocgd_TopologyAnalyzer::is_point_inside(const Vector3& point, const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check point inside - shape reference is null");
            return false;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check point inside - shape is null");
            return false;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot check point inside - OpenCASCADE shape is null");
            return false;
        }

        gp_Pnt occt_point(point.x, point.y, point.z);

        BRepClass3d_SolidClassifier classifier(occt_shape, occt_point, 1e-7);
        return classifier.State() == TopAbs_IN;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception checking point inside - " + String(e.GetMessageString()));
        set_error(String("Point inside check failed: ") + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception checking point inside - " + String(e.what()));
        set_error(String("Point inside check failed: ") + String(e.what()));
        return false;
    }
}

Dictionary ocgd_TopologyAnalyzer::compare_shapes(const Ref<ocgd_TopoDS_Shape>& shape1,
                                                const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;

    if (shape1.is_null() || shape2.is_null()) {
        return result;
    }

    try {
        Dictionary topo1 = get_topology_summary(shape1);
        Dictionary topo2 = get_topology_summary(shape2);

        Dictionary geom1 = analyze_geometric_properties(shape1);
        Dictionary geom2 = analyze_geometric_properties(shape2);

        result["topology_similarity"] = 1.0; // Simplified - would need detailed comparison
        result["geometric_similarity"] = 1.0; // Simplified - would need detailed comparison

        result["volume_ratio"] = (double)geom2.get("volume", 1.0) / (double)geom1.get("volume", 1.0);
        result["area_ratio"] = (double)geom2.get("surface_area", 1.0) / (double)geom1.get("surface_area", 1.0);

        result["topology_difference"] = Dictionary(); // Would contain detailed differences

    } catch (const Standard_Failure& e) {
        set_error(String("Shape comparison failed: ") + String(e.GetMessageString()));
    }

    return result;
}

// Utility methods
void ocgd_TopologyAnalyzer::clear_cache() {
    _cache_valid = false;
    _cached_analysis.clear();
    _cached_shape = Ref<ocgd_TopoDS_Shape>();
}

String ocgd_TopologyAnalyzer::get_last_error() const {
    return _last_error;
}

// Private helper methods
Dictionary ocgd_TopologyAnalyzer::count_topology_elements(const TopoDS_Shape& shape) const {
    Dictionary result;

    try {
        if (shape.IsNull()) {
            UtilityFunctions::printerr("TopologyAnalyzer: Cannot count topology elements - shape is null");
            return result;
        }

        TopTools_IndexedMapOfShape solids, shells, faces, wires, edges, vertices;

        TopExp::MapShapes(shape, TopAbs_SOLID, solids);
        TopExp::MapShapes(shape, TopAbs_SHELL, shells);
        TopExp::MapShapes(shape, TopAbs_FACE, faces);
        TopExp::MapShapes(shape, TopAbs_WIRE, wires);
        TopExp::MapShapes(shape, TopAbs_EDGE, edges);
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertices);

        result["solids"] = solids.Extent();
        result["shells"] = shells.Extent();
        result["faces"] = faces.Extent();
        result["wires"] = wires.Extent();
        result["edges"] = edges.Extent();
        result["vertices"] = vertices.Extent();

        result["shape_type"] = shape_type_to_string(shape.ShapeType());
        result["orientation"] = orientation_to_string(shape.Orientation());

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception counting topology elements - " + String(e.GetMessageString()));
        set_error(String("Topology counting failed: ") + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("TopologyAnalyzer: Exception counting topology elements - " + String(e.what()));
        set_error(String("Topology counting failed: ") + String(e.what()));
    }

    return result;
}

ocgd_TopologyAnalyzer::SurfaceClass ocgd_TopologyAnalyzer::classify_surface(const TopoDS_Face& face) const {
    try {
        BRepAdaptor_Surface surface(face);
        GeomAbs_SurfaceType surface_type = surface.GetType();

        switch (surface_type) {
            case GeomAbs_Plane:
                return SURFACE_PLANE;
            case GeomAbs_Cylinder:
                return SURFACE_CYLINDER;
            case GeomAbs_Cone:
                return SURFACE_CONE;
            case GeomAbs_Sphere:
                return SURFACE_SPHERE;
            case GeomAbs_Torus:
                return SURFACE_TORUS;
            case GeomAbs_BezierSurface:
                return SURFACE_BEZIER;
            case GeomAbs_BSplineSurface:
                return SURFACE_BSPLINE;
            case GeomAbs_SurfaceOfRevolution:
                return SURFACE_REVOLUTION;
            case GeomAbs_SurfaceOfExtrusion:
                return SURFACE_EXTRUSION;
            default:
                return SURFACE_OTHER;
        }
    } catch (const Standard_Failure&) {
        return SURFACE_OTHER;
    }
}

ocgd_TopologyAnalyzer::CurveClass ocgd_TopologyAnalyzer::classify_curve(const TopoDS_Edge& edge) const {
    try {
        BRepAdaptor_Curve curve(edge);
        GeomAbs_CurveType curve_type = curve.GetType();

        switch (curve_type) {
            case GeomAbs_Line:
                return CURVE_LINE;
            case GeomAbs_Circle:
                return CURVE_CIRCLE;
            case GeomAbs_Ellipse:
                return CURVE_ELLIPSE;
            case GeomAbs_Hyperbola:
                return CURVE_HYPERBOLA;
            case GeomAbs_Parabola:
                return CURVE_PARABOLA;
            case GeomAbs_BezierCurve:
                return CURVE_BEZIER;
            case GeomAbs_BSplineCurve:
                return CURVE_BSPLINE;
            default:
                return CURVE_OTHER;
        }
    } catch (const Standard_Failure&) {
        return CURVE_OTHER;
    }
}

Dictionary ocgd_TopologyAnalyzer::analyze_face_properties(const TopoDS_Face& face) const {
    Dictionary result;

    try {
        result["surface_type"] = static_cast<int>(classify_surface(face));
        result["orientation"] = orientation_to_string(face.Orientation());

        // Calculate area
        GProp_GProps props;
        BRepGProp::SurfaceProperties(face, props);
        result["area"] = props.Mass();

        gp_Pnt center = props.CentreOfMass();
        result["center"] = gp_pnt_to_vector3(center);

        // Get UV bounds
        Standard_Real u_min, u_max, v_min, v_max;
        BRepTools::UVBounds(face, u_min, u_max, v_min, v_max);
        result["u_range"] = Array::make(u_min, u_max);
        result["v_range"] = Array::make(v_min, v_max);

        result["tolerance"] = BRep_Tool::Tolerance(face);

    } catch (const Standard_Failure& e) {
        set_error(String("Face properties analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Dictionary ocgd_TopologyAnalyzer::analyze_edge_properties(const TopoDS_Edge& edge) const {
    Dictionary result;

    try {
        result["curve_type"] = static_cast<int>(classify_curve(edge));
        result["orientation"] = orientation_to_string(edge.Orientation());

        // Calculate length
        GProp_GProps props;
        BRepGProp::LinearProperties(edge, props);
        result["length"] = props.Mass();

        // Get parameter range
        Standard_Real first, last;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
        if (!curve.IsNull()) {
            result["parameter_range"] = Array::make(first, last);

            // Get start and end points
            gp_Pnt start_pnt = curve->Value(first);
            gp_Pnt end_pnt = curve->Value(last);
            result["start_point"] = gp_pnt_to_vector3(start_pnt);
            result["end_point"] = gp_pnt_to_vector3(end_pnt);
        }

        result["tolerance"] = BRep_Tool::Tolerance(edge);
        result["is_degenerated"] = BRep_Tool::Degenerated(edge);

    } catch (const Standard_Failure& e) {
        set_error(String("Edge properties analysis failed: ") + String(e.GetMessageString()));
    }

    return result;
}

Array ocgd_TopologyAnalyzer::find_face_neighbors(const TopoDS_Shape& shape, const TopoDS_Face& face) const {
    Array neighbors;

    try {
        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);

        std::unordered_set<int> neighbor_indices;

        TopExp_Explorer edge_explorer(face, TopAbs_EDGE);
        while (edge_explorer.More()) {
            const TopoDS_Edge& edge = TopoDS::Edge(edge_explorer.Current());

            // Find this edge in the map
            for (int i = 1; i <= edge_face_map.Extent(); i++) {
                if (edge_face_map.FindKey(i).IsSame(edge)) {
                    const TopTools_ListOfShape& face_list = edge_face_map.FindFromIndex(i);

                    // Add all faces connected to this edge (except the current face)
                    TopTools_ListIteratorOfListOfShape it(face_list);
                    while (it.More()) {
                        if (!it.Value().IsSame(face)) {
                            // Find the index of this neighbor face
                            TopExp_Explorer face_explorer_all(shape, TopAbs_FACE);
                            int face_index = 0;
                            while (face_explorer_all.More()) {
                                if (face_explorer_all.Current().IsSame(it.Value())) {
                                    neighbor_indices.insert(face_index);
                                    break;
                                }
                                face_explorer_all.Next();
                                face_index++;
                            }
                        }
                        it.Next();
                    }
                    break;
                }
            }

            edge_explorer.Next();
        }

        for (int idx : neighbor_indices) {
            neighbors.append(idx);
        }

    } catch (const Standard_Failure& e) {
        set_error(String("Face neighbor search failed: ") + String(e.GetMessageString()));
    }

    return neighbors;
}

void ocgd_TopologyAnalyzer::set_error(const String& error) const {
    _last_error = error;
    UtilityFunctions::printerr("TopologyAnalyzer: " + error);
}

Vector3 ocgd_TopologyAnalyzer::gp_pnt_to_vector3(const gp_Pnt& point) const {
    return Vector3(
        static_cast<real_t>(point.X()),
        static_cast<real_t>(point.Y()),
        static_cast<real_t>(point.Z())
    );
}

Vector3 ocgd_TopologyAnalyzer::gp_dir_to_vector3(const gp_Dir& direction) const {
    return Vector3(
        static_cast<real_t>(direction.X()),
        static_cast<real_t>(direction.Y()),
        static_cast<real_t>(direction.Z())
    );
}

String ocgd_TopologyAnalyzer::shape_type_to_string(TopAbs_ShapeEnum type) const {
    switch (type) {
        case TopAbs_COMPOUND: return "Compound";
        case TopAbs_COMPSOLID: return "CompSolid";
        case TopAbs_SOLID: return "Solid";
        case TopAbs_SHELL: return "Shell";
        case TopAbs_FACE: return "Face";
        case TopAbs_WIRE: return "Wire";
        case TopAbs_EDGE: return "Edge";
        case TopAbs_VERTEX: return "Vertex";
        case TopAbs_SHAPE: return "Shape";
        default: return "Unknown";
    }
}

String ocgd_TopologyAnalyzer::orientation_to_string(TopAbs_Orientation orientation) const {
    switch (orientation) {
        case TopAbs_FORWARD: return "Forward";
        case TopAbs_REVERSED: return "Reversed";
        case TopAbs_INTERNAL: return "Internal";
        case TopAbs_EXTERNAL: return "External";
        default: return "Unknown";
    }
}
