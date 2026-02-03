#include "ocgd_measurement_tool.h"
#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/error_macros.hpp>

// OpenCASCADE includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomLProp_CLProps.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopoDS.hxx>
#include <BRepGProp_Face.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <TopoDS_Wire.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Standard_Failure.hxx>
#include <Precision.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <math.h>

using namespace godot;

ocgd_measurement_tool::ocgd_measurement_tool() {
    precision_tolerance = Precision::Confusion();
    use_high_precision = false;
    include_detailed_info = true;
    measurement_units = "mm";
    unit_scale_factor = 1.0;
    validate_inputs = true;
    last_error = "";
}

ocgd_measurement_tool::~ocgd_measurement_tool() {
}

void ocgd_measurement_tool::_bind_methods() {
    ClassDB::bind_static_method("ocgd_measurement_tool", D_METHOD("new_tool"), &ocgd_measurement_tool::new_tool);
    
    // Basic distance measurements
    ClassDB::bind_method(D_METHOD("measure_distance_3d", "point1", "point2"), &ocgd_measurement_tool::measure_distance_3d);
    ClassDB::bind_method(D_METHOD("measure_distance_to_shape", "point", "shape"), &ocgd_measurement_tool::measure_distance_to_shape);
    ClassDB::bind_method(D_METHOD("measure_distance_between_shapes", "shape1", "shape2"), &ocgd_measurement_tool::measure_distance_between_shapes);
    
    // Advanced distance measurements
    ClassDB::bind_method(D_METHOD("measure_distance_detailed", "point1", "point2"), &ocgd_measurement_tool::measure_distance_detailed);
    ClassDB::bind_method(D_METHOD("measure_closest_points", "shape1", "shape2"), &ocgd_measurement_tool::measure_closest_points);
    ClassDB::bind_method(D_METHOD("measure_minimum_distance_analysis", "shape1", "shape2"), &ocgd_measurement_tool::measure_minimum_distance_analysis);
    
    // Angular measurements
    ClassDB::bind_method(D_METHOD("measure_angle_between_vectors", "vector1", "vector2"), &ocgd_measurement_tool::measure_angle_between_vectors);
    ClassDB::bind_method(D_METHOD("measure_angle_between_planes", "plane1", "plane2"), &ocgd_measurement_tool::measure_angle_between_planes);
    ClassDB::bind_method(D_METHOD("measure_angle_between_edges", "shape", "edge_index1", "edge_index2"), &ocgd_measurement_tool::measure_angle_between_edges);
    ClassDB::bind_method(D_METHOD("measure_dihedral_angle", "shape", "face_index1", "face_index2"), &ocgd_measurement_tool::measure_dihedral_angle);
    
    // Surface and volume measurements
    ClassDB::bind_method(D_METHOD("measure_surface_area", "shape"), &ocgd_measurement_tool::measure_surface_area);
    ClassDB::bind_method(D_METHOD("measure_volume", "shape"), &ocgd_measurement_tool::measure_volume);
    ClassDB::bind_method(D_METHOD("measure_face_area", "shape", "face_index"), &ocgd_measurement_tool::measure_face_area);
    ClassDB::bind_method(D_METHOD("measure_edge_length", "shape", "edge_index"), &ocgd_measurement_tool::measure_edge_length);
    
    // Geometric property measurements
    ClassDB::bind_method(D_METHOD("measure_center_of_mass", "shape"), &ocgd_measurement_tool::measure_center_of_mass);
    ClassDB::bind_method(D_METHOD("measure_centroid", "shape"), &ocgd_measurement_tool::measure_centroid);
    ClassDB::bind_method(D_METHOD("measure_moments_of_inertia", "shape"), &ocgd_measurement_tool::measure_moments_of_inertia);
    
    // Bounding measurements
    ClassDB::bind_method(D_METHOD("measure_bounding_box", "shape"), &ocgd_measurement_tool::measure_bounding_box);
    ClassDB::bind_method(D_METHOD("measure_bounding_box_volume", "shape"), &ocgd_measurement_tool::measure_bounding_box_volume);
    ClassDB::bind_method(D_METHOD("measure_bounding_sphere", "shape"), &ocgd_measurement_tool::measure_bounding_sphere);
    
    // Curvature measurements
    ClassDB::bind_method(D_METHOD("measure_curvature_at_point", "shape", "face_index", "u", "v"), &ocgd_measurement_tool::measure_curvature_at_point);
    ClassDB::bind_method(D_METHOD("measure_gaussian_curvature", "shape", "face_index", "u", "v"), &ocgd_measurement_tool::measure_gaussian_curvature);
    ClassDB::bind_method(D_METHOD("measure_mean_curvature", "shape", "face_index", "u", "v"), &ocgd_measurement_tool::measure_mean_curvature);
    
    // Specialized CAD measurements
    ClassDB::bind_method(D_METHOD("measure_hole_diameter", "shape", "face_index"), &ocgd_measurement_tool::measure_hole_diameter);
    ClassDB::bind_method(D_METHOD("measure_fillet_radius", "shape", "face_index"), &ocgd_measurement_tool::measure_fillet_radius);
    ClassDB::bind_method(D_METHOD("measure_wall_thickness", "shape", "point", "direction"), &ocgd_measurement_tool::measure_wall_thickness);
    
    // Clearance and interference
    ClassDB::bind_method(D_METHOD("measure_clearance", "shape1", "shape2"), &ocgd_measurement_tool::measure_clearance);
    ClassDB::bind_method(D_METHOD("measure_interference_analysis", "shape1", "shape2"), &ocgd_measurement_tool::measure_interference_analysis);
    
    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_precision_tolerance", "tolerance"), &ocgd_measurement_tool::set_precision_tolerance);
    ClassDB::bind_method(D_METHOD("get_precision_tolerance"), &ocgd_measurement_tool::get_precision_tolerance);
    ClassDB::bind_method(D_METHOD("set_use_high_precision", "high_precision"), &ocgd_measurement_tool::set_use_high_precision);
    ClassDB::bind_method(D_METHOD("get_use_high_precision"), &ocgd_measurement_tool::get_use_high_precision);
    ClassDB::bind_method(D_METHOD("set_measurement_units", "units"), &ocgd_measurement_tool::set_measurement_units);
    ClassDB::bind_method(D_METHOD("get_measurement_units"), &ocgd_measurement_tool::get_measurement_units);
    ClassDB::bind_method(D_METHOD("set_unit_scale_factor", "scale"), &ocgd_measurement_tool::set_unit_scale_factor);
    ClassDB::bind_method(D_METHOD("get_unit_scale_factor"), &ocgd_measurement_tool::get_unit_scale_factor);
    
    // Validation methods
    ClassDB::bind_method(D_METHOD("validate_shape", "shape"), &ocgd_measurement_tool::validate_shape);
    ClassDB::bind_method(D_METHOD("validate_face_index", "shape", "face_index"), &ocgd_measurement_tool::validate_face_index);
    ClassDB::bind_method(D_METHOD("validate_edge_index", "shape", "edge_index"), &ocgd_measurement_tool::validate_edge_index);
    
    // Unit conversion methods
    ClassDB::bind_method(D_METHOD("convert_length", "value", "from_units", "to_units"), &ocgd_measurement_tool::convert_length);
    ClassDB::bind_method(D_METHOD("convert_area", "value", "from_units", "to_units"), &ocgd_measurement_tool::convert_area);
    ClassDB::bind_method(D_METHOD("convert_volume", "value", "from_units", "to_units"), &ocgd_measurement_tool::convert_volume);
    ClassDB::bind_method(D_METHOD("convert_angle", "value", "from_units", "to_units"), &ocgd_measurement_tool::convert_angle);
    
    // Error handling
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_measurement_tool::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_measurement_tool::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_measurement_tool::clear_error);
    
    // Utility
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_measurement_tool::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_measurement_tool::is_null);
}

Ref<ocgd_measurement_tool> ocgd_measurement_tool::new_tool() {
    return memnew(ocgd_measurement_tool);
}

// Basic distance measurements
double ocgd_measurement_tool::measure_distance_3d(const Vector3& point1, const Vector3& point2) {
    try {
        gp_Pnt p1(point1.x, point1.y, point1.z);
        gp_Pnt p2(point2.x, point2.y, point2.z);
        double distance = p1.Distance(p2);
        return distance * unit_scale_factor;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring 3D distance: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring 3D distance: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring 3D distance";
        ERR_PRINT(last_error);
    }
    return -1.0;
}

double ocgd_measurement_tool::measure_distance_to_shape(const Vector3& point, const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), -1.0, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return -1.0;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        gp_Pnt query_point(point.x, point.y, point.z);
        
        BRepExtrema_DistShapeShape dist_calc;
        BRepBuilderAPI_MakeVertex vertex_maker(query_point);
        dist_calc.LoadS1(vertex_maker.Vertex());
        dist_calc.LoadS2(occ_shape);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            double distance = dist_calc.Value();
            return distance * unit_scale_factor;
        }
        
        last_error = "Failed to calculate distance to shape";
        ERR_PRINT(last_error);
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring distance to shape: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring distance to shape: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring distance to shape";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_measurement_tool::measure_distance_between_shapes(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) {
    ERR_FAIL_NULL_V_MSG(shape1.ptr(), -1.0, "Shape1 is null");
    ERR_FAIL_NULL_V_MSG(shape2.ptr(), -1.0, "Shape2 is null");
    
    if (validate_inputs) {
        if (!validate_shape(shape1) || !validate_shape(shape2)) {
            return -1.0;
        }
    }
    
    try {
        TopoDS_Shape occ_shape1 = shape1->get_shape();
        TopoDS_Shape occ_shape2 = shape2->get_shape();
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(occ_shape1);
        dist_calc.LoadS2(occ_shape2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            double distance = dist_calc.Value();
            return distance * unit_scale_factor;
        }
        
        last_error = "Failed to calculate distance between shapes";
        ERR_PRINT(last_error);
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring distance between shapes: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring distance between shapes: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring distance between shapes";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

Dictionary ocgd_measurement_tool::measure_distance_detailed(const Vector3& point1, const Vector3& point2) {
    Dictionary result;
    
    try {
        gp_Pnt p1(point1.x, point1.y, point1.z);
        gp_Pnt p2(point2.x, point2.y, point2.z);
        
        double distance = p1.Distance(p2) * unit_scale_factor;
        Vector3 direction = (point2 - point1).normalized();
        Vector3 midpoint = (point1 + point2) * 0.5;
        
        result["distance"] = distance;
        result["direction"] = direction;
        result["midpoint"] = midpoint;
        result["point1"] = point1;
        result["point2"] = point2;
        result["units"] = measurement_units;
        
        if (include_detailed_info) {
            result["dx"] = abs(point2.x - point1.x) * unit_scale_factor;
            result["dy"] = abs(point2.y - point1.y) * unit_scale_factor;
            result["dz"] = abs(point2.z - point1.z) * unit_scale_factor;
            result["distance_squared"] = distance * distance;
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error in detailed distance measurement: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception in detailed distance measurement: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception in detailed distance measurement";
        ERR_PRINT(last_error);
    }
    
    return result;
}

Dictionary ocgd_measurement_tool::measure_closest_points(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) {
    Dictionary result;
    ERR_FAIL_NULL_V_MSG(shape1.ptr(), result, "Shape1 is null");
    ERR_FAIL_NULL_V_MSG(shape2.ptr(), result, "Shape2 is null");
    
    if (validate_inputs) {
        if (!validate_shape(shape1) || !validate_shape(shape2)) {
            return result;
        }
    }
    
    try {
        TopoDS_Shape occ_shape1 = shape1->get_shape();
        TopoDS_Shape occ_shape2 = shape2->get_shape();
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(occ_shape1);
        dist_calc.LoadS2(occ_shape2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            double distance = dist_calc.Value() * unit_scale_factor;
            
            gp_Pnt p1 = dist_calc.PointOnShape1(1);
            gp_Pnt p2 = dist_calc.PointOnShape2(1);
            
            result["distance"] = distance;
            result["point_on_shape1"] = Vector3(p1.X(), p1.Y(), p1.Z());
            result["point_on_shape2"] = Vector3(p2.X(), p2.Y(), p2.Z());
            result["units"] = measurement_units;
            result["num_solutions"] = dist_calc.NbSolution();
            
            if (include_detailed_info) {
                Array all_solutions;
                for (int i = 1; i <= dist_calc.NbSolution(); i++) {
                    Dictionary solution;
                    gp_Pnt pt1 = dist_calc.PointOnShape1(i);
                    gp_Pnt pt2 = dist_calc.PointOnShape2(i);
                    solution["point1"] = Vector3(pt1.X(), pt1.Y(), pt1.Z());
                    solution["point2"] = Vector3(pt2.X(), pt2.Y(), pt2.Z());
                    all_solutions.append(solution);
                }
                result["all_solutions"] = all_solutions;
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring closest points: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring closest points: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring closest points";
        ERR_PRINT(last_error);
    }
    
    return result;
}

// Angular measurements
double ocgd_measurement_tool::measure_angle_between_vectors(const Vector3& vector1, const Vector3& vector2) {
    try {
        gp_Vec v1(vector1.x, vector1.y, vector1.z);
        gp_Vec v2(vector2.x, vector2.y, vector2.z);
        
        if (v1.Magnitude() < precision_tolerance || v2.Magnitude() < precision_tolerance) {
            last_error = "One or both vectors have zero magnitude";
            ERR_PRINT(last_error);
            return -1.0;
        }
        
        double angle = v1.Angle(v2);
        return angle; // Returns angle in radians
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring angle between vectors: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring angle between vectors: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring angle between vectors";
        ERR_PRINT(last_error);
    }
    return -1.0;
}

double ocgd_measurement_tool::measure_angle_between_planes(const Ref<ocgd_shape>& plane1, const Ref<ocgd_shape>& plane2) {
    ERR_FAIL_NULL_V_MSG(plane1.ptr(), -1.0, "Plane1 is null");
    ERR_FAIL_NULL_V_MSG(plane2.ptr(), -1.0, "Plane2 is null");
    
    try {
        TopoDS_Shape shape1 = plane1->get_shape();
        TopoDS_Shape shape2 = plane2->get_shape();
        
        // Get the first face from each shape
        TopExp_Explorer exp1(shape1, TopAbs_FACE);
        TopExp_Explorer exp2(shape2, TopAbs_FACE);
        
        if (!exp1.More() || !exp2.More()) {
            last_error = "One or both shapes have no faces";
            ERR_PRINT(last_error);
            return -1.0;
        }
        
        TopoDS_Face face1 = TopoDS::Face(exp1.Current());
        TopoDS_Face face2 = TopoDS::Face(exp2.Current());
        
        BRepAdaptor_Surface surf1(face1);
        BRepAdaptor_Surface surf2(face2);
        
        if (surf1.GetType() != GeomAbs_Plane || surf2.GetType() != GeomAbs_Plane) {
            last_error = "One or both surfaces are not planar";
            ERR_PRINT(last_error);
            return -1.0;
        }
        
        gp_Pln plane_1 = surf1.Plane();
        gp_Pln plane_2 = surf2.Plane();
        
        gp_Dir normal1 = plane_1.Axis().Direction();
        gp_Dir normal2 = plane_2.Axis().Direction();
        
        double angle = normal1.Angle(normal2);
        return angle; // Returns angle in radians
        
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring angle between planes: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring angle between planes: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring angle between planes";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

// Surface and volume measurements
double ocgd_measurement_tool::measure_surface_area(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), -1.0, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return -1.0;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        
        GProp_GProps props;
        BRepGProp::SurfaceProperties(occ_shape, props);
        
        double area = props.Mass() * unit_scale_factor * unit_scale_factor;
        return area;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring surface area: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring surface area: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring surface area";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_measurement_tool::measure_volume(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), -1.0, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return -1.0;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        
        GProp_GProps props;
        BRepGProp::VolumeProperties(occ_shape, props);
        
        double volume = props.Mass() * unit_scale_factor * unit_scale_factor * unit_scale_factor;
        return volume;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring volume: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring volume: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring volume";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_measurement_tool::measure_face_area(const Ref<ocgd_shape>& shape, int face_index) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), -1.0, "Shape is null");
    
    if (validate_inputs && !validate_face_index(shape, face_index)) {
        return -1.0;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(occ_shape, TopAbs_FACE, face_map);
        
        ERR_FAIL_INDEX_V_MSG(face_index, face_map.Extent(), -1.0, "Face index out of range");
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        GProp_GProps props;
        BRepGProp::SurfaceProperties(face, props);
        
        double area = props.Mass() * unit_scale_factor * unit_scale_factor;
        return area;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring face area: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring face area: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring face area";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_measurement_tool::measure_edge_length(const Ref<ocgd_shape>& shape, int edge_index) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), -1.0, "Shape is null");
    
    if (validate_inputs && !validate_edge_index(shape, edge_index)) {
        return -1.0;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(occ_shape, TopAbs_EDGE, edge_map);
        
        ERR_FAIL_INDEX_V_MSG(edge_index, edge_map.Extent(), -1.0, "Edge index out of range");
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        GProp_GProps props;
        BRepGProp::LinearProperties(edge, props);
        
        double length = props.Mass() * unit_scale_factor;
        return length;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring edge length: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring edge length: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring edge length";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

// Geometric property measurements
Vector3 ocgd_measurement_tool::measure_center_of_mass(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_V_MSG(shape.ptr(), Vector3(), "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return Vector3();
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        
        GProp_GProps props;
        BRepGProp::VolumeProperties(occ_shape, props);
        
        gp_Pnt center = props.CentreOfMass();
        return Vector3(center.X(), center.Y(), center.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring center of mass: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring center of mass: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring center of mass";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

Vector3 ocgd_measurement_tool::measure_centroid(const Ref<ocgd_shape>& shape) {
    // For most purposes, centroid and center of mass are the same
    return measure_center_of_mass(shape);
}

Dictionary ocgd_measurement_tool::measure_moments_of_inertia(const Ref<ocgd_shape>& shape) {
    Dictionary moments;
    ERR_FAIL_NULL_V_MSG(shape.ptr(), moments, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return moments;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        
        GProp_GProps props;
        BRepGProp::VolumeProperties(occ_shape, props);
        
        gp_Mat inertia = props.MatrixOfInertia();
        
        moments["ixx"] = inertia(1,1);
        moments["ixy"] = inertia(1,2);
        moments["ixz"] = inertia(1,3);
        moments["iyy"] = inertia(2,2);
        moments["iyz"] = inertia(2,3);
        moments["izz"] = inertia(3,3);
        
        gp_Pnt center = props.CentreOfMass();
        moments["center_of_mass"] = Vector3(center.X(), center.Y(), center.Z());
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring moments of inertia: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring moments of inertia: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring moments of inertia";
        ERR_PRINT(last_error);
    }
    
    return moments;
}

// Bounding measurements
Dictionary ocgd_measurement_tool::measure_bounding_box(const Ref<ocgd_shape>& shape) {
    Dictionary bbox;
    ERR_FAIL_NULL_V_MSG(shape.ptr(), bbox, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return bbox;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        
        Bnd_Box box;
        BRepBndLib::Add(occ_shape, box);
        
        if (!box.IsVoid()) {
            double xmin, ymin, zmin, xmax, ymax, zmax;
            box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            
            bbox["min_x"] = xmin * unit_scale_factor;
            bbox["min_y"] = ymin * unit_scale_factor;
            bbox["min_z"] = zmin * unit_scale_factor;
            bbox["max_x"] = xmax * unit_scale_factor;
            bbox["max_y"] = ymax * unit_scale_factor;
            bbox["max_z"] = zmax * unit_scale_factor;
            bbox["size_x"] = (xmax - xmin) * unit_scale_factor;
            bbox["size_y"] = (ymax - ymin) * unit_scale_factor;
            bbox["size_z"] = (zmax - zmin) * unit_scale_factor;
            bbox["center"] = Vector3((xmin + xmax) * 0.5, (ymin + ymax) * 0.5, (zmin + zmax) * 0.5);
            bbox["volume"] = (xmax - xmin) * (ymax - ymin) * (zmax - zmin) * unit_scale_factor * unit_scale_factor * unit_scale_factor;
            bbox["units"] = measurement_units;
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring bounding box: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring bounding box: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring bounding box";
        ERR_PRINT(last_error);
    }
    
    return bbox;
}

double ocgd_measurement_tool::measure_bounding_box_volume(const Ref<ocgd_shape>& shape) {
    Dictionary bbox = measure_bounding_box(shape);
    if (bbox.has("volume")) {
        return bbox["volume"];
    }
    return -1.0;
}

Dictionary ocgd_measurement_tool::measure_bounding_sphere(const Ref<ocgd_shape>& shape) {
    Dictionary sphere;
    ERR_FAIL_NULL_V_MSG(shape.ptr(), sphere, "Shape is null");
    
    if (validate_inputs && !validate_shape(shape)) {
        return sphere;
    }
    
    try {
        Dictionary bbox = measure_bounding_box(shape);
        if (bbox.has("center") && bbox.has("size_x") && bbox.has("size_y") && bbox.has("size_z")) {
            Vector3 center = bbox["center"];
            double size_x = bbox["size_x"];
            double size_y = bbox["size_y"];
            double size_z = bbox["size_z"];
            
            // Simple approximation: radius is half the diagonal of the bounding box
            double radius = sqrt(size_x * size_x + size_y * size_y + size_z * size_z) * 0.5;
            
            sphere["center"] = center;
            sphere["radius"] = radius;
            sphere["diameter"] = radius * 2.0;
            sphere["volume"] = (4.0 / 3.0) * M_PI * radius * radius * radius;
            sphere["surface_area"] = 4.0 * M_PI * radius * radius;
            sphere["units"] = measurement_units;
        }
        
        clear_error();
    } catch (...) {
        last_error = "Error calculating bounding sphere";
        ERR_PRINT(last_error);
    }
    
    return sphere;
}

// Validation methods
bool ocgd_measurement_tool::validate_shape(const Ref<ocgd_shape>& shape) {
    if (shape.is_null()) {
        last_error = "Shape is null";
        ERR_PRINT(last_error);
        return false;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        if (occ_shape.IsNull()) {
            last_error = "OpenCASCADE shape is null";
            ERR_PRINT(last_error);
            return false;
        }
        
        BRepCheck_Analyzer analyzer(occ_shape);
        if (!analyzer.IsValid()) {
            last_error = "Shape is invalid";
            ERR_PRINT(last_error);
            return false;
        }
        
        return true;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error validating shape: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown error validating shape";
        ERR_PRINT(last_error);
    }
    
    return false;
}

bool ocgd_measurement_tool::validate_face_index(const Ref<ocgd_shape>& shape, int face_index) {
    if (!validate_shape(shape)) {
        return false;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(occ_shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            last_error = String("Face index ") + String::num(face_index) + " out of range [0, " + String::num(face_map.Extent() - 1) + "]";
            ERR_PRINT(last_error);
            return false;
        }
        
        return true;
    } catch (...) {
        last_error = "Error validating face index";
        ERR_PRINT(last_error);
    }
    
    return false;
}

bool ocgd_measurement_tool::validate_edge_index(const Ref<ocgd_shape>& shape, int edge_index) {
    if (!validate_shape(shape)) {
        return false;
    }
    
    try {
        TopoDS_Shape occ_shape = shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(occ_shape, TopAbs_EDGE, edge_map);
        
        if (edge_index < 0 || edge_index >= edge_map.Extent()) {
            last_error = String("Edge index ") + String::num(edge_index) + " out of range [0, " + String::num(edge_map.Extent() - 1) + "]";
            ERR_PRINT(last_error);
            return false;
        }
        
        return true;
    } catch (...) {
        last_error = "Error validating edge index";
        ERR_PRINT(last_error);
    }
    
    return false;
}

// Configuration methods
void ocgd_measurement_tool::set_precision_tolerance(double tolerance) {
    ERR_FAIL_COND_MSG(tolerance <= 0.0, "Precision tolerance must be positive");
    precision_tolerance = tolerance;
}

double ocgd_measurement_tool::get_precision_tolerance() const {
    return precision_tolerance;
}

void ocgd_measurement_tool::set_use_high_precision(bool high_precision) {
    use_high_precision = high_precision;
    if (high_precision) {
        precision_tolerance = Precision::Confusion() * 0.1;
    } else {
        precision_tolerance = Precision::Confusion();
    }
}

bool ocgd_measurement_tool::get_use_high_precision() const {
    return use_high_precision;
}

void ocgd_measurement_tool::set_measurement_units(const String& units) {
    measurement_units = units;
    
    // Set scale factor based on units (assuming input is in mm)
    if (units == "mm") {
        unit_scale_factor = 1.0;
    } else if (units == "cm") {
        unit_scale_factor = 0.1;
    } else if (units == "m") {
        unit_scale_factor = 0.001;
    } else if (units == "in") {
        unit_scale_factor = 1.0 / 25.4;
    } else if (units == "ft") {
        unit_scale_factor = 1.0 / (25.4 * 12.0);
    } else {
        unit_scale_factor = 1.0; // Default to mm
    }
}

String ocgd_measurement_tool::get_measurement_units() const {
    return measurement_units;
}

void ocgd_measurement_tool::set_unit_scale_factor(double scale) {
    ERR_FAIL_COND_MSG(scale <= 0.0, "Unit scale factor must be positive");
    unit_scale_factor = scale;
}

double ocgd_measurement_tool::get_unit_scale_factor() const {
    return unit_scale_factor;
}

void ocgd_measurement_tool::set_validate_inputs(bool validate) {
    validate_inputs = validate;
}

bool ocgd_measurement_tool::get_validate_inputs() const {
    return validate_inputs;
}

// Unit conversion methods
double ocgd_measurement_tool::convert_length(double value, const String& from_units, const String& to_units) {
    // Convert to mm first, then to target units
    double mm_value = value;
    
    // Convert from source units to mm
    if (from_units == "cm") {
        mm_value = value * 10.0;
    } else if (from_units == "m") {
        mm_value = value * 1000.0;
    } else if (from_units == "in") {
        mm_value = value * 25.4;
    } else if (from_units == "ft") {
        mm_value = value * 25.4 * 12.0;
    }
    
    // Convert from mm to target units
    if (to_units == "mm") {
        return mm_value;
    } else if (to_units == "cm") {
        return mm_value / 10.0;
    } else if (to_units == "m") {
        return mm_value / 1000.0;
    } else if (to_units == "in") {
        return mm_value / 25.4;
    } else if (to_units == "ft") {
        return mm_value / (25.4 * 12.0);
    }
    
    return mm_value; // Default to mm
}

double ocgd_measurement_tool::convert_area(double value, const String& from_units, const String& to_units) {
    double length_conversion = convert_length(1.0, from_units, to_units);
    return value * length_conversion * length_conversion;
}

double ocgd_measurement_tool::convert_volume(double value, const String& from_units, const String& to_units) {
    double length_conversion = convert_length(1.0, from_units, to_units);
    return value * length_conversion * length_conversion * length_conversion;
}

double ocgd_measurement_tool::convert_angle(double value, const String& from_units, const String& to_units) {
    double rad_value = value;
    
    // Convert to radians first
    if (from_units == "deg" || from_units == "degrees") {
        rad_value = value * M_PI / 180.0;
    } else if (from_units == "grad" || from_units == "gradians") {
        rad_value = value * M_PI / 200.0;
    }
    
    // Convert from radians to target
    if (to_units == "rad" || to_units == "radians") {
        return rad_value;
    } else if (to_units == "deg" || to_units == "degrees") {
        return rad_value * 180.0 / M_PI;
    } else if (to_units == "grad" || to_units == "gradians") {
        return rad_value * 200.0 / M_PI;
    }
    
    return rad_value; // Default to radians
}

// Error handling
String ocgd_measurement_tool::get_last_error() const {
    return last_error;
}

bool ocgd_measurement_tool::has_error() const {
    return !last_error.is_empty();
}

void ocgd_measurement_tool::clear_error() {
    last_error = "";
}

String ocgd_measurement_tool::get_type() const {
    return "ocgd_measurement_tool";
}

bool ocgd_measurement_tool::is_null() const {
    return false;
}

// Advanced measurement implementations
Dictionary ocgd_measurement_tool::measure_minimum_distance_analysis(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) {
    Dictionary result;
    ERR_FAIL_COND_V_MSG(shape1.is_null() || shape2.is_null(), result, "One or both shapes are null");

    try {
        TopoDS_Shape oc_shape1 = shape1->get_shape();
        TopoDS_Shape oc_shape2 = shape2->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape1.IsNull() || oc_shape2.IsNull(), result, "One or both shapes are invalid");

        BRepExtrema_DistShapeShape dist_calc(oc_shape1, oc_shape2);
        dist_calc.Perform();

        if (dist_calc.IsDone()) {
            result["distance"] = dist_calc.Value();
            result["point_count"] = dist_calc.NbSolution();
            
            if (dist_calc.NbSolution() > 0) {
                gp_Pnt pt1 = dist_calc.PointOnShape1(1);
                gp_Pnt pt2 = dist_calc.PointOnShape2(1);
                result["point1"] = Vector3(pt1.X(), pt1.Y(), pt1.Z());
                result["point2"] = Vector3(pt2.X(), pt2.Y(), pt2.Z());
            }
        } else {
            result["error"] = "Distance calculation failed";
        }
    } catch (const Standard_Failure& e) {
        result["error"] = "OpenCASCADE error: " + String(e.GetMessageString());
    } catch (...) {
        result["error"] = "Unknown error during distance analysis";
    }

    return result;
}

double ocgd_measurement_tool::measure_angle_between_edges(const Ref<ocgd_shape>& shape, int edge_index1, int edge_index2) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), -1.0, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), -1.0, "Shape is invalid");

        TopExp_Explorer edge_explorer(oc_shape, TopAbs_EDGE);
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(oc_shape, TopAbs_EDGE, edge_map);

        ERR_FAIL_INDEX_V_MSG(edge_index1, edge_map.Extent(), -1.0, "Edge index 1 out of range");
        ERR_FAIL_INDEX_V_MSG(edge_index2, edge_map.Extent(), -1.0, "Edge index 2 out of range");

        TopoDS_Edge edge1 = TopoDS::Edge(edge_map(edge_index1 + 1));
        TopoDS_Edge edge2 = TopoDS::Edge(edge_map(edge_index2 + 1));

        // Get curve adapters
        BRepAdaptor_Curve curve1(edge1);
        BRepAdaptor_Curve curve2(edge2);

        // Get tangent vectors at midpoints
        double u1 = (curve1.FirstParameter() + curve1.LastParameter()) / 2.0;
        double u2 = (curve2.FirstParameter() + curve2.LastParameter()) / 2.0;

        gp_Vec tangent1, tangent2;
        gp_Pnt point1, point2;
        curve1.D1(u1, point1, tangent1);
        curve2.D1(u2, point2, tangent2);

        // Calculate angle between tangent vectors
        double angle = tangent1.Angle(tangent2);
        return angle * 180.0 / M_PI; // Convert to degrees

    } catch (const Standard_Failure& e) {
        ERR_PRINT("OpenCASCADE error: " + String(e.GetMessageString()));
        return -1.0;
    } catch (...) {
        ERR_PRINT("Unknown error during edge angle measurement");
        return -1.0;
    }
}

Dictionary ocgd_measurement_tool::measure_dihedral_angle(const Ref<ocgd_shape>& shape, int face_index1, int face_index2) {
    Dictionary result;
    ERR_FAIL_COND_V_MSG(shape.is_null(), result, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), result, "Shape is invalid");

        TopExp_Explorer face_explorer(oc_shape, TopAbs_FACE);
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(oc_shape, TopAbs_FACE, face_map);

        ERR_FAIL_INDEX_V_MSG(face_index1, face_map.Extent(), result, "Face index 1 out of range");
        ERR_FAIL_INDEX_V_MSG(face_index2, face_map.Extent(), result, "Face index 2 out of range");

        TopoDS_Face face1 = TopoDS::Face(face_map(face_index1 + 1));
        TopoDS_Face face2 = TopoDS::Face(face_map(face_index2 + 1));

        // Get surface normals at centers
        BRepGProp_Face prop1(face1);
        BRepGProp_Face prop2(face2);

        gp_Pnt center1, center2;
        gp_Vec normal1, normal2;
        
        // Get surface parameters at center
        Standard_Real u1, v1, u2, v2;
        BRepTools::UVBounds(face1, u1, u2, v1, v2);
        Standard_Real mid_u1 = (u1 + u2) / 2.0;
        Standard_Real mid_v1 = (v1 + v2) / 2.0;

        BRepTools::UVBounds(face2, u1, u2, v1, v2);
        Standard_Real mid_u2 = (u1 + u2) / 2.0;
        Standard_Real mid_v2 = (v1 + v2) / 2.0;

        prop1.Normal(mid_u1, mid_v1, center1, normal1);
        prop2.Normal(mid_u2, mid_v2, center2, normal2);

        // Calculate dihedral angle
        double angle = normal1.Angle(normal2);
        result["angle_radians"] = angle;
        result["angle_degrees"] = angle * 180.0 / M_PI;
        result["center1"] = Vector3(center1.X(), center1.Y(), center1.Z());
        result["center2"] = Vector3(center2.X(), center2.Y(), center2.Z());
        result["normal1"] = Vector3(normal1.X(), normal1.Y(), normal1.Z());
        result["normal2"] = Vector3(normal2.X(), normal2.Y(), normal2.Z());

    } catch (const Standard_Failure& e) {
        result["error"] = "OpenCASCADE error: " + String(e.GetMessageString());
    } catch (...) {
        result["error"] = "Unknown error during dihedral angle measurement";
    }

    return result;
}

double ocgd_measurement_tool::measure_wire_length(const Ref<ocgd_shape>& shape, int wire_index) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), -1.0, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), -1.0, "Shape is invalid");

        TopExp_Explorer wire_explorer(oc_shape, TopAbs_WIRE);
        TopTools_IndexedMapOfShape wire_map;
        TopExp::MapShapes(oc_shape, TopAbs_WIRE, wire_map);

        ERR_FAIL_INDEX_V_MSG(wire_index, wire_map.Extent(), -1.0, "Wire index out of range");

        TopoDS_Wire wire = TopoDS::Wire(wire_map(wire_index + 1));
        
        // Calculate wire length by summing edge lengths
        double total_length = 0.0;
        TopExp_Explorer edge_explorer(wire, TopAbs_EDGE);
        while (edge_explorer.More()) {
            TopoDS_Edge edge = TopoDS::Edge(edge_explorer.Current());
            BRepAdaptor_Curve curve(edge);
            total_length += GCPnts_AbscissaPoint::Length(curve);
            edge_explorer.Next();
        }

        return total_length;

    } catch (const Standard_Failure& e) {
        ERR_PRINT("OpenCASCADE error: " + String(e.GetMessageString()));
        return -1.0;
    } catch (...) {
        ERR_PRINT("Unknown error during wire length measurement");
        return -1.0;
    }
}

double ocgd_measurement_tool::measure_perimeter(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_COND_V_MSG(shape.is_null(), -1.0, "Shape is null");

    try {
        TopoDS_Shape oc_shape = shape->get_shape();
        ERR_FAIL_COND_V_MSG(oc_shape.IsNull(), -1.0, "Shape is invalid");

        double total_perimeter = 0.0;
        
        // Sum lengths of all edges
        TopExp_Explorer edge_explorer(oc_shape, TopAbs_EDGE);
        while (edge_explorer.More()) {
            TopoDS_Edge edge = TopoDS::Edge(edge_explorer.Current());
            BRepAdaptor_Curve curve(edge);
            total_perimeter += GCPnts_AbscissaPoint::Length(curve);
            edge_explorer.Next();
        }

        return total_perimeter;

    } catch (const Standard_Failure& e) {
        ERR_PRINT("OpenCASCADE error: " + String(e.GetMessageString()));
        return -1.0;
    } catch (...) {
        ERR_PRINT("Unknown error during perimeter measurement");
        return -1.0;
    }
}
double ocgd_measurement_tool::measure_face_perimeter(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
double ocgd_measurement_tool::measure_circumference(const Ref<ocgd_shape>& shape) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_principal_axes(const Ref<ocgd_shape>& shape) { return Dictionary(); }
Dictionary ocgd_measurement_tool::measure_oriented_bounding_box(const Ref<ocgd_shape>& shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_bounding_sphere_radius(const Ref<ocgd_shape>& shape) { return -1.0; }
double ocgd_measurement_tool::measure_curvature_at_point(const Ref<ocgd_shape>& shape, int face_index, double u, double v) { return 0.0; }
Dictionary ocgd_measurement_tool::measure_curvature_analysis(const Ref<ocgd_shape>& shape, int face_index) { return Dictionary(); }
double ocgd_measurement_tool::measure_gaussian_curvature(const Ref<ocgd_shape>& shape, int face_index, double u, double v) { return 0.0; }
double ocgd_measurement_tool::measure_mean_curvature(const Ref<ocgd_shape>& shape, int face_index, double u, double v) { return 0.0; }
Dictionary ocgd_measurement_tool::measure_principal_curvatures(const Ref<ocgd_shape>& shape, int face_index, double u, double v) { return Dictionary(); }
double ocgd_measurement_tool::measure_hole_diameter(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
double ocgd_measurement_tool::measure_fillet_radius(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
double ocgd_measurement_tool::measure_chamfer_distance(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_thread_pitch(const Ref<ocgd_shape>& shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_wall_thickness(const Ref<ocgd_shape>& shape, const Vector3& point, const Vector3& direction) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_thickness_analysis(const Ref<ocgd_shape>& shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_minimum_wall_thickness(const Ref<ocgd_shape>& shape) { return -1.0; }
Array ocgd_measurement_tool::measure_thickness_variations(const Ref<ocgd_shape>& shape, int num_samples) { return Array(); }
Dictionary ocgd_measurement_tool::measure_cross_section_area(const Ref<ocgd_shape>& shape, const Vector3& plane_point, const Vector3& plane_normal) { return Dictionary(); }
Dictionary ocgd_measurement_tool::measure_cross_section_properties(const Ref<ocgd_shape>& shape, const Vector3& plane_point, const Vector3& plane_normal) { return Dictionary(); }
double ocgd_measurement_tool::measure_hydraulic_diameter(const Ref<ocgd_shape>& shape, const Vector3& plane_point, const Vector3& plane_normal) { return -1.0; }
double ocgd_measurement_tool::measure_draft_angle(const Ref<ocgd_shape>& shape, int face_index, const Vector3& draft_direction) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_taper_analysis(const Ref<ocgd_shape>& shape, const Vector3& axis_direction) { return Dictionary(); }
Array ocgd_measurement_tool::measure_undercuts(const Ref<ocgd_shape>& shape, const Vector3& draft_direction) { return Array(); }
double ocgd_measurement_tool::measure_clearance(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) { return measure_distance_between_shapes(shape1, shape2); }
Dictionary ocgd_measurement_tool::measure_interference_analysis(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) { return Dictionary(); }
Array ocgd_measurement_tool::measure_interference_points(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) { return Array(); }
double ocgd_measurement_tool::measure_penetration_depth(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_fit_analysis(const Ref<ocgd_shape>& hole_shape, const Ref<ocgd_shape>& shaft_shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_concentricity_error(const Ref<ocgd_shape>& shape1, const Ref<ocgd_shape>& shape2) { return -1.0; }
double ocgd_measurement_tool::measure_parallelism_error(const Ref<ocgd_shape>& shape, int face_index1, int face_index2) { return -1.0; }
double ocgd_measurement_tool::measure_perpendicularity_error(const Ref<ocgd_shape>& shape, int face_index1, int face_index2) { return -1.0; }
double ocgd_measurement_tool::measure_flatness_error(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
double ocgd_measurement_tool::measure_cylindricity_error(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
double ocgd_measurement_tool::measure_sphericity_error(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_in_coordinate_system(const Ref<ocgd_shape>& shape, const Vector3& origin, const Vector3& x_axis, const Vector3& y_axis) { return Dictionary(); }
Vector3 ocgd_measurement_tool::transform_to_coordinate_system(const Vector3& point, const Vector3& origin, const Vector3& x_axis, const Vector3& y_axis) { return Vector3(); }
Vector3 ocgd_measurement_tool::transform_from_coordinate_system(const Vector3& point, const Vector3& origin, const Vector3& x_axis, const Vector3& y_axis) { return Vector3(); }
Dictionary ocgd_measurement_tool::measure_statistical_analysis(const Array& measurements) { return Dictionary(); }
double ocgd_measurement_tool::measure_standard_deviation(const Array& measurements) { return -1.0; }
double ocgd_measurement_tool::measure_variance(const Array& measurements) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_distribution_analysis(const Array& measurements) { return Dictionary(); }
Array ocgd_measurement_tool::measure_multiple_distances(const Array& point_pairs) { return Array(); }
Array ocgd_measurement_tool::measure_multiple_areas(const Ref<ocgd_shape>& shape, const Array& face_indices) { return Array(); }
Array ocgd_measurement_tool::measure_multiple_volumes(const Array& shapes) { return Array(); }
Dictionary ocgd_measurement_tool::measure_comparative_analysis(const Array& shapes) { return Dictionary(); }
void ocgd_measurement_tool::set_include_detailed_info(bool detailed) { include_detailed_info = detailed; }
bool ocgd_measurement_tool::get_include_detailed_info() const { return include_detailed_info; }
bool ocgd_measurement_tool::validate_measurement_parameters(const Dictionary& parameters) { return true; }
Dictionary ocgd_measurement_tool::export_measurement_report(const Dictionary& measurements) { return Dictionary(); }
String ocgd_measurement_tool::generate_measurement_summary(const Dictionary& measurements) { return ""; }
bool ocgd_measurement_tool::save_measurement_report(const Dictionary& measurements, const String& file_path) { return false; }
Array ocgd_measurement_tool::get_supported_units() const { Array units; units.append("mm"); units.append("cm"); units.append("m"); units.append("in"); units.append("ft"); return units; }
Dictionary ocgd_measurement_tool::measure_form_factor(const Ref<ocgd_shape>& shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_aspect_ratio(const Ref<ocgd_shape>& shape) { return -1.0; }
double ocgd_measurement_tool::measure_compactness(const Ref<ocgd_shape>& shape) { return -1.0; }
double ocgd_measurement_tool::measure_sphericity(const Ref<ocgd_shape>& shape) { return -1.0; }
Dictionary ocgd_measurement_tool::measure_shape_complexity(const Ref<ocgd_shape>& shape) { return Dictionary(); }
Dictionary ocgd_measurement_tool::measure_geometric_tolerance_analysis(const Ref<ocgd_shape>& shape) { return Dictionary(); }
Array ocgd_measurement_tool::measure_feature_consistency(const Array& shapes) { return Array(); }
Dictionary ocgd_measurement_tool::measure_manufacturing_feasibility(const Ref<ocgd_shape>& shape) { return Dictionary(); }
double ocgd_measurement_tool::measure_surface_roughness_estimate(const Ref<ocgd_shape>& shape, int face_index) { return -1.0; }