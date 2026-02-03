/**
 * ocgd_ShapeAnalyzer.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE shape analysis and measurement tools.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_ShapeAnalyzer.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Mat.hxx>
#include <opencascade/Bnd_Box.hxx>
#include <opencascade/BRepBndLib.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopExp.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/Precision.hxx>


using namespace godot;

ocgd_ShapeAnalyzer::ocgd_ShapeAnalyzer() {
    _use_triangulation = false;
    _skip_shared_edges = false;
    _linear_deflection = 0.1;
    _angular_deflection = 0.5;
    _high_precision_mode = false;
}

ocgd_ShapeAnalyzer::~ocgd_ShapeAnalyzer() {
    // No cleanup needed
}

void ocgd_ShapeAnalyzer::set_use_triangulation(bool use_triangulation) {
    _use_triangulation = use_triangulation;
}

bool ocgd_ShapeAnalyzer::get_use_triangulation() const {
    return _use_triangulation;
}

void ocgd_ShapeAnalyzer::set_skip_shared_edges(bool skip_shared) {
    _skip_shared_edges = skip_shared;
}

bool ocgd_ShapeAnalyzer::get_skip_shared_edges() const {
    return _skip_shared_edges;
}

void ocgd_ShapeAnalyzer::set_linear_deflection(double deflection) {
    _linear_deflection = deflection;
}

double ocgd_ShapeAnalyzer::get_linear_deflection() const {
    return _linear_deflection;
}

void ocgd_ShapeAnalyzer::set_angular_deflection(double deflection) {
    _angular_deflection = deflection;
}

double ocgd_ShapeAnalyzer::get_angular_deflection() const {
    return _angular_deflection;
}

void ocgd_ShapeAnalyzer::set_high_precision_mode(bool high_precision) {
    _high_precision_mode = high_precision;
}

bool ocgd_ShapeAnalyzer::get_high_precision_mode() const {
    return _high_precision_mode;
}

Dictionary ocgd_ShapeAnalyzer::compute_volume_properties(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute volume properties - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute volume properties - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute volume properties - OpenCASCADE shape is null");
            return result;
        }

        GProp_GProps volume_props;
        BRepGProp::VolumeProperties(occt_shape, volume_props, _use_triangulation);

        return gprops_to_dictionary(volume_props);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing volume properties - " + String(e.GetMessageString()));
        return result;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing volume properties - " + String(e.what()));
        return result;
    }
}

Dictionary ocgd_ShapeAnalyzer::compute_surface_properties(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute surface properties - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute surface properties - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute surface properties - OpenCASCADE shape is null");
            return result;
        }

        GProp_GProps surface_props;
        BRepGProp::SurfaceProperties(occt_shape, surface_props, _use_triangulation, _skip_shared_edges);

        return gprops_to_dictionary(surface_props);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing surface properties - " + String(e.GetMessageString()));
        return result;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing surface properties - " + String(e.what()));
        return result;
    }
}

Dictionary ocgd_ShapeAnalyzer::compute_linear_properties(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute linear properties - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute linear properties - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute linear properties - OpenCASCADE shape is null");
            return result;
        }

        GProp_GProps linear_props;
        BRepGProp::LinearProperties(occt_shape, linear_props, _use_triangulation, _skip_shared_edges);

        return gprops_to_dictionary(linear_props);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing linear properties - " + String(e.GetMessageString()));
        return result;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("ShapeAnalyzer: Exception computing linear properties - " + String(e.what()));
        return result;
    }
}

double ocgd_ShapeAnalyzer::get_volume(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary props = compute_volume_properties(shape);
    if (props.has("mass")) {
        return props["mass"];
    }
    return 0.0;
}

double ocgd_ShapeAnalyzer::get_surface_area(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary props = compute_surface_properties(shape);
    if (props.has("mass")) {
        return props["mass"];
    }
    return 0.0;
}

double ocgd_ShapeAnalyzer::get_total_length(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary props = compute_linear_properties(shape);
    if (props.has("mass")) {
        return props["mass"];
    }
    return 0.0;
}

Vector3 ocgd_ShapeAnalyzer::get_center_of_mass(const Ref<ocgd_TopoDS_Shape>& shape) {
    // Try volume properties first, then surface, then linear
    Dictionary volume_props = compute_volume_properties(shape);
    if (volume_props.has("center_of_mass")) {
        return volume_props["center_of_mass"];
    }

    Dictionary surface_props = compute_surface_properties(shape);
    if (surface_props.has("center_of_mass")) {
        return surface_props["center_of_mass"];
    }

    Dictionary linear_props = compute_linear_properties(shape);
    if (linear_props.has("center_of_mass")) {
        return linear_props["center_of_mass"];
    }

    return Vector3(0, 0, 0);
}

Dictionary ocgd_ShapeAnalyzer::compute_inertia_properties(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& reference_point) {
    Dictionary result;

    if (shape.is_null() || shape->is_null()) {
        UtilityFunctions::printerr("ShapeAnalyzer: Cannot compute inertia properties of null shape");
        return result;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    GProp_GProps props;

    // Use volume properties for solids, surface for others
    TopExp_Explorer solid_explorer(occt_shape, TopAbs_SOLID);
    if (solid_explorer.More()) {
        BRepGProp::VolumeProperties(occt_shape, props, _use_triangulation);
    } else {
        BRepGProp::SurfaceProperties(occt_shape, props, _skip_shared_edges, _use_triangulation);
    }

    // Set reference point if different from origin
    if (reference_point.length() > Precision::Confusion()) {
        gp_Pnt ref_pnt(reference_point.x, reference_point.y, reference_point.z);
        // SetLocation is not available in the current API
        // The reference point should be set during calculation
    }

    // Extract inertia matrix
    gp_Mat inertia_matrix = props.MatrixOfInertia();
    Array matrix_values;
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            matrix_values.append(inertia_matrix.Value(i, j));
        }
    }
    result["inertia_matrix"] = matrix_values;

    // Get principal moments and axes
    gp_Vec principal_axis1, principal_axis2, principal_axis3;
    Standard_Real moment1, moment2, moment3;
    GProp_PrincipalProps principal_props = props.PrincipalProperties();
    principal_props.Moments(moment1, moment2, moment3);
    principal_axis1 = principal_props.FirstAxisOfInertia();
    principal_axis2 = principal_props.SecondAxisOfInertia();
    principal_axis3 = principal_props.ThirdAxisOfInertia();

    Array principal_moments;
    principal_moments.append(moment1);
    principal_moments.append(moment2);
    principal_moments.append(moment3);
    result["principal_moments"] = principal_moments;

    Array principal_axes;
    principal_axes.append(Vector3(principal_axis1.X(), principal_axis1.Y(), principal_axis1.Z()));
    principal_axes.append(Vector3(principal_axis2.X(), principal_axis2.Y(), principal_axis2.Z()));
    principal_axes.append(Vector3(principal_axis3.X(), principal_axis3.Y(), principal_axis3.Z()));
    result["principal_axes"] = principal_axes;

    // Add basic properties
    result["mass"] = props.Mass();
    gp_Pnt center = props.CentreOfMass();
    result["center_of_mass"] = Vector3(center.X(), center.Y(), center.Z());

    return result;
}

double ocgd_ShapeAnalyzer::get_moment_about_axis(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& axis_origin, const Vector3& axis_direction) {
    if (shape.is_null() || shape->is_null()) {
        return 0.0;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    GProp_GProps props;

    // Use appropriate property calculation
    TopExp_Explorer solid_explorer(occt_shape, TopAbs_SOLID);
    if (solid_explorer.More()) {
        BRepGProp::VolumeProperties(occt_shape, props, _use_triangulation);
    } else {
        BRepGProp::SurfaceProperties(occt_shape, props, _skip_shared_edges, _use_triangulation);
    }

    gp_Pnt origin(axis_origin.x, axis_origin.y, axis_origin.z);
    gp_Vec direction(axis_direction.x, axis_direction.y, axis_direction.z);
    gp_Ax1 axis(origin, direction);

    return props.MomentOfInertia(axis);
}

double ocgd_ShapeAnalyzer::get_radius_of_gyration(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& axis_origin, const Vector3& axis_direction) {
    if (shape.is_null() || shape->is_null()) {
        return 0.0;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    GProp_GProps props;

    TopExp_Explorer solid_explorer(occt_shape, TopAbs_SOLID);
    if (solid_explorer.More()) {
        BRepGProp::VolumeProperties(occt_shape, props, _use_triangulation);
    } else {
        BRepGProp::SurfaceProperties(occt_shape, props, _skip_shared_edges, _use_triangulation);
    }

    gp_Pnt origin(axis_origin.x, axis_origin.y, axis_origin.z);
    gp_Vec direction(axis_direction.x, axis_direction.y, axis_direction.z);
    gp_Ax1 axis(origin, direction);

    return props.RadiusOfGyration(axis);
}

Array ocgd_ShapeAnalyzer::get_principal_moments(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary inertia_props = compute_inertia_properties(shape);
    if (inertia_props.has("principal_moments")) {
        return inertia_props["principal_moments"];
    }
    return Array();
}

Array ocgd_ShapeAnalyzer::get_principal_axes(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary inertia_props = compute_inertia_properties(shape);
    if (inertia_props.has("principal_axes")) {
        return inertia_props["principal_axes"];
    }
    return Array();
}

Dictionary ocgd_ShapeAnalyzer::get_bounding_box(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null() || shape->is_null()) {
        result["min"] = Vector3(0, 0, 0);
        result["max"] = Vector3(0, 0, 0);
        return result;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    Bnd_Box bounding_box;
    BRepBndLib::Add(occt_shape, bounding_box);

    if (bounding_box.IsVoid()) {
        result["min"] = Vector3(0, 0, 0);
        result["max"] = Vector3(0, 0, 0);
        return result;
    }

    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    bounding_box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    result["min"] = Vector3(
        static_cast<float>(xmin),
        static_cast<float>(ymin),
        static_cast<float>(zmin)
    );
    result["max"] = Vector3(
        static_cast<float>(xmax),
        static_cast<float>(ymax),
        static_cast<float>(zmax)
    );

    return result;
}

Vector3 ocgd_ShapeAnalyzer::get_dimensions(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary bbox = get_bounding_box(shape);
    Vector3 min_pt = bbox["min"];
    Vector3 max_pt = bbox["max"];

    return Vector3(
        max_pt.x - min_pt.x,
        max_pt.y - min_pt.y,
        max_pt.z - min_pt.z
    );
}

double ocgd_ShapeAnalyzer::get_max_dimension(const Ref<ocgd_TopoDS_Shape>& shape) {
    Vector3 dims = get_dimensions(shape);
    return std::max({dims.x, dims.y, dims.z});
}

double ocgd_ShapeAnalyzer::get_min_dimension(const Ref<ocgd_TopoDS_Shape>& shape) {
    Vector3 dims = get_dimensions(shape);
    return std::min({dims.x, dims.y, dims.z});
}

Dictionary ocgd_ShapeAnalyzer::validate_shape(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null() || shape->is_null()) {
        result["is_valid"] = false;
        result["errors"] = Array();
        result["warnings"] = Array();
        return result;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    BRepCheck_Analyzer analyzer(occt_shape);

    result["is_valid"] = analyzer.IsValid() == Standard_True;

    // For now, just return basic validation
    // More detailed error analysis would require more complex implementation
    Array errors, warnings;
    if (!analyzer.IsValid()) {
        errors.append("Shape has geometric inconsistencies");
    }

    result["errors"] = errors;
    result["warnings"] = warnings;

    return result;
}

bool ocgd_ShapeAnalyzer::is_valid_solid(const Ref<ocgd_TopoDS_Shape>& shape) {
    if (shape.is_null() || shape->is_null()) {
        return false;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    if (occt_shape.ShapeType() != TopAbs_SOLID) {
        return false;
    }

    BRepCheck_Analyzer analyzer(occt_shape);
    return analyzer.IsValid() == Standard_True;
}

double ocgd_ShapeAnalyzer::get_distance_between_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    if (shape1.is_null() || shape1->is_null() || shape2.is_null() || shape2->is_null()) {
        return -1.0; // Invalid distance
    }

    const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
    const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();

    BRepExtrema_DistShapeShape distance_calc(occt_shape1, occt_shape2);
    distance_calc.Perform();

    if (distance_calc.IsDone()) {
        return distance_calc.Value();
    }

    return -1.0;
}

Dictionary ocgd_ShapeAnalyzer::get_closest_points(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2) {
    Dictionary result;

    if (shape1.is_null() || shape1->is_null() || shape2.is_null() || shape2->is_null()) {
        result["distance"] = -1.0;
        return result;
    }

    const TopoDS_Shape& occt_shape1 = shape1->get_occt_shape();
    const TopoDS_Shape& occt_shape2 = shape2->get_occt_shape();

    BRepExtrema_DistShapeShape distance_calc(occt_shape1, occt_shape2);
    distance_calc.Perform();

    if (distance_calc.IsDone() && distance_calc.NbSolution() > 0) {
        gp_Pnt point1 = distance_calc.PointOnShape1(1);
        gp_Pnt point2 = distance_calc.PointOnShape2(1);

        result["point1"] = Vector3(point1.X(), point1.Y(), point1.Z());
        result["point2"] = Vector3(point2.X(), point2.Y(), point2.Z());
        result["distance"] = distance_calc.Value();
    } else {
        result["distance"] = -1.0;
    }

    return result;
}

Dictionary ocgd_ShapeAnalyzer::get_topology_counts(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary result;

    if (shape.is_null() || shape->is_null()) {
        result["vertices"] = 0;
        result["edges"] = 0;
        result["faces"] = 0;
        result["solids"] = 0;
        result["shells"] = 0;
        result["wires"] = 0;
        result["compounds"] = 0;
        return result;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    TopTools_IndexedMapOfShape vertex_map, edge_map, face_map, solid_map, shell_map, wire_map, compound_map;

    TopExp::MapShapes(occt_shape, TopAbs_VERTEX, vertex_map);
    TopExp::MapShapes(occt_shape, TopAbs_EDGE, edge_map);
    TopExp::MapShapes(occt_shape, TopAbs_FACE, face_map);
    TopExp::MapShapes(occt_shape, TopAbs_SOLID, solid_map);
    TopExp::MapShapes(occt_shape, TopAbs_SHELL, shell_map);
    TopExp::MapShapes(occt_shape, TopAbs_WIRE, wire_map);
    TopExp::MapShapes(occt_shape, TopAbs_COMPOUND, compound_map);

    result["vertices"] = vertex_map.Extent();
    result["edges"] = edge_map.Extent();
    result["faces"] = face_map.Extent();
    result["solids"] = solid_map.Extent();
    result["shells"] = shell_map.Extent();
    result["wires"] = wire_map.Extent();
    result["compounds"] = compound_map.Extent();

    return result;
}

Dictionary ocgd_ShapeAnalyzer::get_full_analysis_report(const Ref<ocgd_TopoDS_Shape>& shape) {
    Dictionary report;

    if (shape.is_null() || shape->is_null()) {
        report["error"] = "Cannot analyze null shape";
        return report;
    }

    // Basic properties
    report["bounding_box"] = get_bounding_box(shape);
    report["dimensions"] = get_dimensions(shape);
    report["center_of_mass"] = get_center_of_mass(shape);

    // Measurements
    report["volume"] = get_volume(shape);
    report["surface_area"] = get_surface_area(shape);
    report["total_length"] = get_total_length(shape);

    // Topology
    report["topology_counts"] = get_topology_counts(shape);

    // Validation
    report["validation"] = validate_shape(shape);

    // Inertia (if it's a solid)
    TopExp_Explorer solid_explorer(shape->get_occt_shape(), TopAbs_SOLID);
    if (solid_explorer.More()) {
        report["inertia_properties"] = compute_inertia_properties(shape);
    }

    return report;
}

Dictionary ocgd_ShapeAnalyzer::gprops_to_dictionary(const GProp_GProps& props) const {
    Dictionary result;

    result["mass"] = props.Mass();

    gp_Pnt center = props.CentreOfMass();
    result["center_of_mass"] = Vector3(
        static_cast<float>(center.X()),
        static_cast<float>(center.Y()),
        static_cast<float>(center.Z())
    );

    // Add moments of inertia if available
    gp_Mat inertia_matrix = props.MatrixOfInertia();
    Array matrix_values;
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            matrix_values.append(inertia_matrix.Value(i, j));
        }
    }
    result["inertia_matrix"] = matrix_values;

    return result;
}

void ocgd_ShapeAnalyzer::_bind_methods() {
    // Configuration properties
    ClassDB::bind_method(D_METHOD("set_use_triangulation", "use_triangulation"), &ocgd_ShapeAnalyzer::set_use_triangulation);
    ClassDB::bind_method(D_METHOD("get_use_triangulation"), &ocgd_ShapeAnalyzer::get_use_triangulation);
    ClassDB::add_property("ocgd_ShapeAnalyzer", PropertyInfo(Variant::BOOL, "use_triangulation"), "set_use_triangulation", "get_use_triangulation");

    ClassDB::bind_method(D_METHOD("set_skip_shared_edges", "skip_shared"), &ocgd_ShapeAnalyzer::set_skip_shared_edges);
    ClassDB::bind_method(D_METHOD("get_skip_shared_edges"), &ocgd_ShapeAnalyzer::get_skip_shared_edges);
    ClassDB::add_property("ocgd_ShapeAnalyzer", PropertyInfo(Variant::BOOL, "skip_shared_edges"), "set_skip_shared_edges", "get_skip_shared_edges");

    ClassDB::bind_method(D_METHOD("set_linear_deflection", "deflection"), &ocgd_ShapeAnalyzer::set_linear_deflection);
    ClassDB::bind_method(D_METHOD("get_linear_deflection"), &ocgd_ShapeAnalyzer::get_linear_deflection);
    ClassDB::add_property("ocgd_ShapeAnalyzer", PropertyInfo(Variant::FLOAT, "linear_deflection"), "set_linear_deflection", "get_linear_deflection");

    ClassDB::bind_method(D_METHOD("set_angular_deflection", "deflection"), &ocgd_ShapeAnalyzer::set_angular_deflection);
    ClassDB::bind_method(D_METHOD("get_angular_deflection"), &ocgd_ShapeAnalyzer::get_angular_deflection);
    ClassDB::add_property("ocgd_ShapeAnalyzer", PropertyInfo(Variant::FLOAT, "angular_deflection"), "set_angular_deflection", "get_angular_deflection");

    ClassDB::bind_method(D_METHOD("set_high_precision_mode", "high_precision"), &ocgd_ShapeAnalyzer::set_high_precision_mode);
    ClassDB::bind_method(D_METHOD("get_high_precision_mode"), &ocgd_ShapeAnalyzer::get_high_precision_mode);
    ClassDB::add_property("ocgd_ShapeAnalyzer", PropertyInfo(Variant::BOOL, "high_precision_mode"), "set_high_precision_mode", "get_high_precision_mode");

    // Global properties methods
    ClassDB::bind_method(D_METHOD("compute_volume_properties", "shape"), &ocgd_ShapeAnalyzer::compute_volume_properties);
    ClassDB::bind_method(D_METHOD("compute_surface_properties", "shape"), &ocgd_ShapeAnalyzer::compute_surface_properties);
    ClassDB::bind_method(D_METHOD("compute_linear_properties", "shape"), &ocgd_ShapeAnalyzer::compute_linear_properties);

    ClassDB::bind_method(D_METHOD("get_volume", "shape"), &ocgd_ShapeAnalyzer::get_volume);
    ClassDB::bind_method(D_METHOD("get_surface_area", "shape"), &ocgd_ShapeAnalyzer::get_surface_area);
    ClassDB::bind_method(D_METHOD("get_total_length", "shape"), &ocgd_ShapeAnalyzer::get_total_length);
    ClassDB::bind_method(D_METHOD("get_center_of_mass", "shape"), &ocgd_ShapeAnalyzer::get_center_of_mass);

    // Inertia methods
    ClassDB::bind_method(D_METHOD("compute_inertia_properties", "shape", "reference_point"), &ocgd_ShapeAnalyzer::compute_inertia_properties, DEFVAL(Vector3(0, 0, 0)));
    ClassDB::bind_method(D_METHOD("get_moment_about_axis", "shape", "axis_origin", "axis_direction"), &ocgd_ShapeAnalyzer::get_moment_about_axis);
    ClassDB::bind_method(D_METHOD("get_radius_of_gyration", "shape", "axis_origin", "axis_direction"), &ocgd_ShapeAnalyzer::get_radius_of_gyration);
    ClassDB::bind_method(D_METHOD("get_principal_moments", "shape"), &ocgd_ShapeAnalyzer::get_principal_moments);
    ClassDB::bind_method(D_METHOD("get_principal_axes", "shape"), &ocgd_ShapeAnalyzer::get_principal_axes);

    // Bounding and geometry methods
    ClassDB::bind_method(D_METHOD("get_bounding_box", "shape"), &ocgd_ShapeAnalyzer::get_bounding_box);
    ClassDB::bind_method(D_METHOD("get_dimensions", "shape"), &ocgd_ShapeAnalyzer::get_dimensions);
    ClassDB::bind_method(D_METHOD("get_max_dimension", "shape"), &ocgd_ShapeAnalyzer::get_max_dimension);
    ClassDB::bind_method(D_METHOD("get_min_dimension", "shape"), &ocgd_ShapeAnalyzer::get_min_dimension);

    // Validation methods
    ClassDB::bind_method(D_METHOD("validate_shape", "shape"), &ocgd_ShapeAnalyzer::validate_shape);
    ClassDB::bind_method(D_METHOD("is_valid_solid", "shape"), &ocgd_ShapeAnalyzer::is_valid_solid);

    // Distance methods
    ClassDB::bind_method(D_METHOD("get_distance_between_shapes", "shape1", "shape2"), &ocgd_ShapeAnalyzer::get_distance_between_shapes);
    ClassDB::bind_method(D_METHOD("get_closest_points", "shape1", "shape2"), &ocgd_ShapeAnalyzer::get_closest_points);

    // Topology methods
    ClassDB::bind_method(D_METHOD("get_topology_counts", "shape"), &ocgd_ShapeAnalyzer::get_topology_counts);

    // Analysis reports
    ClassDB::bind_method(D_METHOD("get_full_analysis_report", "shape"), &ocgd_ShapeAnalyzer::get_full_analysis_report);
}
