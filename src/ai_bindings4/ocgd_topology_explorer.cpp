#include "ocgd_topology_explorer.h"
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
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomLProp_CLProps.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAbs_CurveType.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>

using namespace godot;

ocgd_topology_explorer::ocgd_topology_explorer() {
    precision_tolerance = Precision::Confusion();
    include_orientation_info = true;
    cache_results = false;
    last_error = "";
}

ocgd_topology_explorer::~ocgd_topology_explorer() {
}

void ocgd_topology_explorer::_bind_methods() {
    ClassDB::bind_static_method("ocgd_topology_explorer", D_METHOD("new_explorer"), &ocgd_topology_explorer::new_explorer);
    
    // Shape management
    ClassDB::bind_method(D_METHOD("set_shape", "shape"), &ocgd_topology_explorer::set_shape);
    ClassDB::bind_method(D_METHOD("get_shape"), &ocgd_topology_explorer::get_shape);
    ClassDB::bind_method(D_METHOD("has_shape"), &ocgd_topology_explorer::has_shape);
    ClassDB::bind_method(D_METHOD("clear_shape"), &ocgd_topology_explorer::clear_shape);
    
    // Basic topology exploration
    ClassDB::bind_method(D_METHOD("get_faces"), &ocgd_topology_explorer::get_faces);
    ClassDB::bind_method(D_METHOD("get_edges"), &ocgd_topology_explorer::get_edges);
    ClassDB::bind_method(D_METHOD("get_vertices"), &ocgd_topology_explorer::get_vertices);
    ClassDB::bind_method(D_METHOD("get_solids"), &ocgd_topology_explorer::get_solids);
    ClassDB::bind_method(D_METHOD("get_shells"), &ocgd_topology_explorer::get_shells);
    ClassDB::bind_method(D_METHOD("get_wires"), &ocgd_topology_explorer::get_wires);
    ClassDB::bind_method(D_METHOD("get_compounds"), &ocgd_topology_explorer::get_compounds);
    
    // Counting methods
    ClassDB::bind_method(D_METHOD("get_face_count"), &ocgd_topology_explorer::get_face_count);
    ClassDB::bind_method(D_METHOD("get_edge_count"), &ocgd_topology_explorer::get_edge_count);
    ClassDB::bind_method(D_METHOD("get_vertex_count"), &ocgd_topology_explorer::get_vertex_count);
    ClassDB::bind_method(D_METHOD("get_solid_count"), &ocgd_topology_explorer::get_solid_count);
    ClassDB::bind_method(D_METHOD("get_shell_count"), &ocgd_topology_explorer::get_shell_count);
    ClassDB::bind_method(D_METHOD("get_wire_count"), &ocgd_topology_explorer::get_wire_count);
    ClassDB::bind_method(D_METHOD("get_compound_count"), &ocgd_topology_explorer::get_compound_count);
    
    // Face properties
    ClassDB::bind_method(D_METHOD("get_face_properties", "face_index"), &ocgd_topology_explorer::get_face_properties);
    ClassDB::bind_method(D_METHOD("get_face_center", "face_index"), &ocgd_topology_explorer::get_face_center);
    ClassDB::bind_method(D_METHOD("get_face_normal", "face_index"), &ocgd_topology_explorer::get_face_normal);
    ClassDB::bind_method(D_METHOD("get_face_area", "face_index"), &ocgd_topology_explorer::get_face_area);
    ClassDB::bind_method(D_METHOD("get_face_bounds", "face_index"), &ocgd_topology_explorer::get_face_bounds);
    ClassDB::bind_method(D_METHOD("get_face_type", "face_index"), &ocgd_topology_explorer::get_face_type);
    ClassDB::bind_method(D_METHOD("is_face_planar", "face_index"), &ocgd_topology_explorer::is_face_planar);
    ClassDB::bind_method(D_METHOD("is_face_closed", "face_index"), &ocgd_topology_explorer::is_face_closed);
    
    // Edge properties
    ClassDB::bind_method(D_METHOD("get_edge_properties", "edge_index"), &ocgd_topology_explorer::get_edge_properties);
    ClassDB::bind_method(D_METHOD("get_edge_start_point", "edge_index"), &ocgd_topology_explorer::get_edge_start_point);
    ClassDB::bind_method(D_METHOD("get_edge_end_point", "edge_index"), &ocgd_topology_explorer::get_edge_end_point);
    ClassDB::bind_method(D_METHOD("get_edge_center", "edge_index"), &ocgd_topology_explorer::get_edge_center);
    ClassDB::bind_method(D_METHOD("get_edge_length", "edge_index"), &ocgd_topology_explorer::get_edge_length);
    ClassDB::bind_method(D_METHOD("get_edge_type", "edge_index"), &ocgd_topology_explorer::get_edge_type);
    ClassDB::bind_method(D_METHOD("is_edge_straight", "edge_index"), &ocgd_topology_explorer::is_edge_straight);
    ClassDB::bind_method(D_METHOD("is_edge_closed", "edge_index"), &ocgd_topology_explorer::is_edge_closed);
    
    // Vertex properties
    ClassDB::bind_method(D_METHOD("get_vertex_properties", "vertex_index"), &ocgd_topology_explorer::get_vertex_properties);
    ClassDB::bind_method(D_METHOD("get_vertex_position", "vertex_index"), &ocgd_topology_explorer::get_vertex_position);
    
    // Distance measurements
    ClassDB::bind_method(D_METHOD("measure_distance_point_to_point", "point1", "point2"), &ocgd_topology_explorer::measure_distance_point_to_point);
    ClassDB::bind_method(D_METHOD("measure_distance_point_to_face", "point", "face_index"), &ocgd_topology_explorer::measure_distance_point_to_face);
    ClassDB::bind_method(D_METHOD("measure_distance_point_to_edge", "point", "edge_index"), &ocgd_topology_explorer::measure_distance_point_to_edge);
    ClassDB::bind_method(D_METHOD("measure_distance_face_to_face", "face_index1", "face_index2"), &ocgd_topology_explorer::measure_distance_face_to_face);
    ClassDB::bind_method(D_METHOD("measure_distance_edge_to_edge", "edge_index1", "edge_index2"), &ocgd_topology_explorer::measure_distance_edge_to_edge);
    ClassDB::bind_method(D_METHOD("measure_distance_vertex_to_vertex", "vertex_index1", "vertex_index2"), &ocgd_topology_explorer::measure_distance_vertex_to_vertex);
    
    // Shape analysis
    ClassDB::bind_method(D_METHOD("analyze_shape_topology"), &ocgd_topology_explorer::analyze_shape_topology);
    ClassDB::bind_method(D_METHOD("get_shape_statistics"), &ocgd_topology_explorer::get_shape_statistics);
    ClassDB::bind_method(D_METHOD("is_shape_valid"), &ocgd_topology_explorer::is_shape_valid);
    ClassDB::bind_method(D_METHOD("is_shape_closed"), &ocgd_topology_explorer::is_shape_closed);
    ClassDB::bind_method(D_METHOD("get_overall_bounding_box"), &ocgd_topology_explorer::get_overall_bounding_box);
    
    // Mass properties
    ClassDB::bind_method(D_METHOD("calculate_mass_properties"), &ocgd_topology_explorer::calculate_mass_properties);
    ClassDB::bind_method(D_METHOD("get_center_of_gravity"), &ocgd_topology_explorer::get_center_of_gravity);
    ClassDB::bind_method(D_METHOD("get_volume"), &ocgd_topology_explorer::get_volume);
    ClassDB::bind_method(D_METHOD("get_surface_area"), &ocgd_topology_explorer::get_surface_area);
    
    // Configuration
    ClassDB::bind_method(D_METHOD("set_precision_tolerance", "tolerance"), &ocgd_topology_explorer::set_precision_tolerance);
    ClassDB::bind_method(D_METHOD("get_precision_tolerance"), &ocgd_topology_explorer::get_precision_tolerance);
    ClassDB::bind_method(D_METHOD("set_include_orientation_info", "include"), &ocgd_topology_explorer::set_include_orientation_info);
    ClassDB::bind_method(D_METHOD("get_include_orientation_info"), &ocgd_topology_explorer::get_include_orientation_info);
    
    // Error handling
    ClassDB::bind_method(D_METHOD("get_last_error"), &ocgd_topology_explorer::get_last_error);
    ClassDB::bind_method(D_METHOD("has_error"), &ocgd_topology_explorer::has_error);
    ClassDB::bind_method(D_METHOD("clear_error"), &ocgd_topology_explorer::clear_error);
    
    // Utility
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_topology_explorer::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_topology_explorer::is_null);
}

Ref<ocgd_topology_explorer> ocgd_topology_explorer::new_explorer() {
    return memnew(ocgd_topology_explorer);
}

void ocgd_topology_explorer::set_shape(const Ref<ocgd_shape>& shape) {
    ERR_FAIL_NULL_MSG(shape.ptr(), "Shape reference is null");
    
    try {
        current_shape = shape;
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error in set_shape: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception in set_shape: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception in set_shape";
        ERR_PRINT(last_error);
    }
}

Ref<ocgd_shape> ocgd_topology_explorer::get_shape() const {
    return current_shape;
}

bool ocgd_topology_explorer::has_shape() const {
    return !current_shape.is_null() && !current_shape->get_shape().IsNull();
}

void ocgd_topology_explorer::clear_shape() {
    current_shape = Ref<ocgd_shape>();
    clear_error();
}

Array ocgd_topology_explorer::get_faces() {
    Array faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), faces, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        ERR_FAIL_COND_V_MSG(shape.IsNull(), faces, "Shape is null");
        
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            faces.append(i - 1); // Return zero-based indices
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting faces: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting faces: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting faces";
        ERR_PRINT(last_error);
    }
    
    return faces;
}

Array ocgd_topology_explorer::get_edges() {
    Array edges;
    ERR_FAIL_COND_V_MSG(!has_shape(), edges, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        ERR_FAIL_COND_V_MSG(shape.IsNull(), edges, "Shape is null");
        
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        for (int i = 1; i <= edge_map.Extent(); i++) {
            edges.append(i - 1); // Return zero-based indices
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edges: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edges: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edges";
        ERR_PRINT(last_error);
    }
    
    return edges;
}

Array ocgd_topology_explorer::get_vertices() {
    Array vertices;
    ERR_FAIL_COND_V_MSG(!has_shape(), vertices, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        ERR_FAIL_COND_V_MSG(shape.IsNull(), vertices, "Shape is null");
        
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        
        for (int i = 1; i <= vertex_map.Extent(); i++) {
            vertices.append(i - 1); // Return zero-based indices
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting vertices: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting vertices: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting vertices";
        ERR_PRINT(last_error);
    }
    
    return vertices;
}

Array ocgd_topology_explorer::get_solids() {
    Array solids;
    ERR_FAIL_COND_V_MSG(!has_shape(), solids, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        ERR_FAIL_COND_V_MSG(shape.IsNull(), solids, "Shape is null");
        
        TopTools_IndexedMapOfShape solid_map;
        TopExp::MapShapes(shape, TopAbs_SOLID, solid_map);
        
        for (int i = 1; i <= solid_map.Extent(); i++) {
            solids.append(i - 1); // Return zero-based indices
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting solids: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting solids: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting solids";
        ERR_PRINT(last_error);
    }
    
    return solids;
}

Array ocgd_topology_explorer::get_shells() {
    Array shells;
    if (!has_shape()) return shells;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape shell_map;
        TopExp::MapShapes(shape, TopAbs_SHELL, shell_map);
        
        for (int i = 1; i <= shell_map.Extent(); i++) {
            shells.append(i - 1);
        }
    } catch (const std::exception& e) {
        last_error = String("Error getting shells: ") + e.what();
    }
    
    return shells;
}

Array ocgd_topology_explorer::get_wires() {
    Array wires;
    if (!has_shape()) return wires;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape wire_map;
        TopExp::MapShapes(shape, TopAbs_WIRE, wire_map);
        
        for (int i = 1; i <= wire_map.Extent(); i++) {
            wires.append(i - 1);
        }
    } catch (const std::exception& e) {
        last_error = String("Error getting wires: ") + e.what();
    }
    
    return wires;
}

Array ocgd_topology_explorer::get_compounds() {
    Array compounds;
    if (!has_shape()) return compounds;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape compound_map;
        TopExp::MapShapes(shape, TopAbs_COMPOUND, compound_map);
        
        for (int i = 1; i <= compound_map.Extent(); i++) {
            compounds.append(i - 1);
        }
    } catch (const std::exception& e) {
        last_error = String("Error getting compounds: ") + e.what();
    }
    
    return compounds;
}

int ocgd_topology_explorer::get_face_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        return face_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_edge_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        return edge_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_vertex_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        return vertex_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_solid_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape solid_map;
        TopExp::MapShapes(shape, TopAbs_SOLID, solid_map);
        return solid_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_shell_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape shell_map;
        TopExp::MapShapes(shape, TopAbs_SHELL, shell_map);
        return shell_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_wire_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape wire_map;
        TopExp::MapShapes(shape, TopAbs_WIRE, wire_map);
        return wire_map.Extent();
    } catch (...) {
        return 0;
    }
}

int ocgd_topology_explorer::get_compound_count() const {
    if (!has_shape()) return 0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape compound_map;
        TopExp::MapShapes(shape, TopAbs_COMPOUND, compound_map);
        return compound_map.Extent();
    } catch (...) {
        return 0;
    }
}

Dictionary ocgd_topology_explorer::get_face_properties(int face_index) {
    Dictionary props;
    if (!has_shape()) {
        last_error = "No shape set for topology exploration";
        return props;
    }
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            last_error = "Face index out of range";
            return props;
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        // Basic properties
        props["index"] = face_index;
        props["center"] = get_face_center(face_index);
        props["normal"] = get_face_normal(face_index);
        props["area"] = get_face_area(face_index);
        props["type"] = get_face_type(face_index);
        props["is_planar"] = is_face_planar(face_index);
        props["is_closed"] = is_face_closed(face_index);
        
        // Orientation
        if (include_orientation_info) {
            props["orientation"] = (face.Orientation() == TopAbs_FORWARD) ? "forward" : "reversed";
        }
        
    } catch (const std::exception& e) {
        last_error = String("Error getting face properties: ") + e.what();
    }
    
    return props;
}

Vector3 ocgd_topology_explorer::get_face_center(int face_index) {
    if (!has_shape()) return Vector3();
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return Vector3();
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        // Calculate center using surface properties
        BRepAdaptor_Surface surface(face);
        double u_min = surface.FirstUParameter();
        double u_max = surface.LastUParameter();
        double v_min = surface.FirstVParameter();
        double v_max = surface.LastVParameter();
        
        double u_mid = (u_min + u_max) / 2.0;
        double v_mid = (v_min + v_max) / 2.0;
        
        gp_Pnt center_point = surface.Value(u_mid, v_mid);
        
        return Vector3(center_point.X(), center_point.Y(), center_point.Z());
        
    } catch (...) {
        return Vector3();
    }
}

Vector3 ocgd_topology_explorer::get_face_normal(int face_index) {
    if (!has_shape()) return Vector3();
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return Vector3();
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        // Calculate normal using surface properties
        BRepAdaptor_Surface surface(face);
        double u_min = surface.FirstUParameter();
        double u_max = surface.LastUParameter();
        double v_min = surface.FirstVParameter();
        double v_max = surface.LastVParameter();
        
        double u_mid = (u_min + u_max) / 2.0;
        double v_mid = (v_min + v_max) / 2.0;
        
        // Get normal using surface derivatives
        gp_Pnt point;
        gp_Vec du, dv;
        surface.D1(u_mid, v_mid, point, du, dv);
        
        // Calculate normal as cross product of partial derivatives
        gp_Vec normal = du.Crossed(dv);
        if (normal.Magnitude() > precision_tolerance) {
            normal.Normalize();
            return Vector3(normal.X(), normal.Y(), normal.Z());
        }
        
    } catch (...) {
        // Fall back to zero vector
    }
    
    return Vector3();
}

double ocgd_topology_explorer::get_face_area(int face_index) {
    if (!has_shape()) return 0.0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return 0.0;
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        GProp_GProps props;
        BRepGProp::SurfaceProperties(face, props);
        return props.Mass();
        
    } catch (...) {
        return 0.0;
    }
}

Dictionary ocgd_topology_explorer::get_face_bounds(int face_index) {
    Dictionary bounds;
    if (!has_shape()) return bounds;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return bounds;
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        Bnd_Box bbox;
        BRepBndLib::Add(face, bbox);
        
        if (!bbox.IsVoid()) {
            double xmin, ymin, zmin, xmax, ymax, zmax;
            bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            
            bounds["min_x"] = xmin;
            bounds["min_y"] = ymin;
            bounds["min_z"] = zmin;
            bounds["max_x"] = xmax;
            bounds["max_y"] = ymax;
            bounds["max_z"] = zmax;
            bounds["size_x"] = xmax - xmin;
            bounds["size_y"] = ymax - ymin;
            bounds["size_z"] = zmax - zmin;
        }
        
    } catch (...) {
        // Return empty bounds
    }
    
    return bounds;
}

String ocgd_topology_explorer::get_face_type(int face_index) {
    if (!has_shape()) return "unknown";
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return "invalid";
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        switch (surface.GetType()) {
            case GeomAbs_Plane: return "plane";
            case GeomAbs_Cylinder: return "cylinder";
            case GeomAbs_Cone: return "cone";
            case GeomAbs_Sphere: return "sphere";
            case GeomAbs_Torus: return "torus";
            case GeomAbs_BezierSurface: return "bezier";
            case GeomAbs_BSplineSurface: return "bspline";
            case GeomAbs_SurfaceOfRevolution: return "revolution";
            case GeomAbs_SurfaceOfExtrusion: return "extrusion";
            default: return "other";
        }
        
    } catch (...) {
        return "error";
    }
}

bool ocgd_topology_explorer::is_face_planar(int face_index) {
    return get_face_type(face_index) == "plane";
}

bool ocgd_topology_explorer::is_face_closed(int face_index) {
    if (!has_shape()) return false;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return false;
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        return surface.IsUClosed() && surface.IsVClosed();
        
    } catch (...) {
        return false;
    }
}

// Distance measurement methods
double ocgd_topology_explorer::measure_distance_point_to_point(const Vector3& point1, const Vector3& point2) {
    gp_Pnt p1(point1.x, point1.y, point1.z);
    gp_Pnt p2(point2.x, point2.y, point2.z);
    return p1.Distance(p2);
}

double ocgd_topology_explorer::measure_distance_point_to_face(const Vector3& point, int face_index) {
    if (!has_shape()) return -1.0;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        if (face_index < 0 || face_index >= face_map.Extent()) {
            return -1.0;
        }
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        gp_Pnt test_point(point.x, point.y, point.z);
        
        BRepExtrema_DistShapeShape dist_calc;
        BRepBuilderAPI_MakeVertex vertex_maker(test_point);
        dist_calc.LoadS1(vertex_maker.Vertex());
        dist_calc.LoadS2(face);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            return dist_calc.Value();
        }
        
    } catch (...) {
        // Error in calculation
    }
    
    return -1.0;
}

String ocgd_topology_explorer::get_last_error() const {
    return last_error;
}

bool ocgd_topology_explorer::has_error() const {
    return !last_error.is_empty();
}

void ocgd_topology_explorer::clear_error() {
    last_error = "";
}

void ocgd_topology_explorer::set_precision_tolerance(double tolerance) {
    precision_tolerance = tolerance;
}

double ocgd_topology_explorer::get_precision_tolerance() const {
    return precision_tolerance;
}

void ocgd_topology_explorer::set_include_orientation_info(bool include) {
    include_orientation_info = include;
}

bool ocgd_topology_explorer::get_include_orientation_info() const {
    return include_orientation_info;
}

String ocgd_topology_explorer::get_type() const {
    return "ocgd_topology_explorer";
}

bool ocgd_topology_explorer::is_null() const {
    return false;
}

// Basic implementations for remaining methods - can be extended as needed
Dictionary ocgd_topology_explorer::analyze_shape_topology() {
    Dictionary analysis;
    if (!has_shape()) return analysis;
    
    analysis["face_count"] = get_face_count();
    analysis["edge_count"] = get_edge_count();
    analysis["vertex_count"] = get_vertex_count();
    analysis["solid_count"] = get_solid_count();
    analysis["shell_count"] = get_shell_count();
    analysis["wire_count"] = get_wire_count();
    analysis["compound_count"] = get_compound_count();
    analysis["is_valid"] = is_shape_valid();
    analysis["is_closed"] = is_shape_closed();
    analysis["bounding_box"] = get_overall_bounding_box();
    
    return analysis;
}

Dictionary ocgd_topology_explorer::get_shape_statistics() {
    return analyze_shape_topology();
}

bool ocgd_topology_explorer::is_shape_valid() {
    if (!has_shape()) return false;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        BRepCheck_Analyzer analyzer(shape);
        return analyzer.IsValid();
    } catch (...) {
        return false;
    }
}

bool ocgd_topology_explorer::is_shape_closed() {
    if (!has_shape()) return false;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        return BRep_Tool::IsClosed(shape);
    } catch (...) {
        return false;
    }
}

Dictionary ocgd_topology_explorer::get_overall_bounding_box() {
    Dictionary bounds;
    if (!has_shape()) return bounds;
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        Bnd_Box bbox;
        BRepBndLib::Add(shape, bbox);
        
        if (!bbox.IsVoid()) {
            double xmin, ymin, zmin, xmax, ymax, zmax;
            bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            
            bounds["min_x"] = xmin;
            bounds["min_y"] = ymin;
            bounds["min_z"] = zmin;
            bounds["max_x"] = xmax;
            bounds["max_y"] = ymax;
            bounds["max_z"] = zmax;
            bounds["size_x"] = xmax - xmin;
            bounds["size_y"] = ymax - ymin;
            bounds["size_z"] = zmax - zmin;
        }
    } catch (...) {
        // Return empty bounds
    }
    
    return bounds;
}

// Edge operations implementation
Dictionary ocgd_topology_explorer::get_edge_properties(int edge_index) {
    Dictionary props;
    ERR_FAIL_COND_V_MSG(!has_shape(), props, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), props, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        props["index"] = edge_index;
        props["start_point"] = get_edge_start_point(edge_index);
        props["end_point"] = get_edge_end_point(edge_index);
        props["center"] = get_edge_center(edge_index);
        props["length"] = get_edge_length(edge_index);
        props["type"] = get_edge_type(edge_index);
        props["is_straight"] = is_edge_straight(edge_index);
        props["is_closed"] = is_edge_closed(edge_index);
        props["tolerance"] = BRep_Tool::Tolerance(edge);
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge properties: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge properties: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge properties";
        ERR_PRINT(last_error);
    }
    
    return props;
}

Vector3 ocgd_topology_explorer::get_edge_start_point(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), Vector3(), "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        TopoDS_Vertex first, last;
        TopExp::Vertices(edge, first, last);
        
        gp_Pnt point = BRep_Tool::Pnt(first);
        return Vector3(point.X(), point.Y(), point.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge start point: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge start point: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge start point";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

Vector3 ocgd_topology_explorer::get_edge_end_point(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), Vector3(), "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        TopoDS_Vertex first, last;
        TopExp::Vertices(edge, first, last);
        
        gp_Pnt point = BRep_Tool::Pnt(last);
        return Vector3(point.X(), point.Y(), point.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge end point: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge end point: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge end point";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

Vector3 ocgd_topology_explorer::get_edge_center(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), Vector3(), "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        double first, last;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
        
        if (!curve.IsNull()) {
            double mid_param = (first + last) / 2.0;
            gp_Pnt center_point = curve->Value(mid_param);
            return Vector3(center_point.X(), center_point.Y(), center_point.Z());
        }
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge center: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge center: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge center";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

double ocgd_topology_explorer::get_edge_length(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), 0.0, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        GProp_GProps props;
        BRepGProp::LinearProperties(edge, props);
        return props.Mass();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge length: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge length: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge length";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}

String ocgd_topology_explorer::get_edge_type(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), "unknown", "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), "unknown", "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        BRepAdaptor_Curve curve(edge);
        
        switch (curve.GetType()) {
            case GeomAbs_Line: return "line";
            case GeomAbs_Circle: return "circle";
            case GeomAbs_Ellipse: return "ellipse";
            case GeomAbs_Hyperbola: return "hyperbola";
            case GeomAbs_Parabola: return "parabola";
            case GeomAbs_BezierCurve: return "bezier";
            case GeomAbs_BSplineCurve: return "bspline";
            case GeomAbs_OffsetCurve: return "offset";
            case GeomAbs_OtherCurve: return "other";
            default: return "unknown";
        }
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge type: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge type: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge type";
        ERR_PRINT(last_error);
    }
    
    return "error";
}

bool ocgd_topology_explorer::is_edge_straight(int edge_index) {
    return get_edge_type(edge_index) == "line";
}

bool ocgd_topology_explorer::is_edge_closed(int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), false, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), false, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        return BRep_Tool::IsClosed(edge);
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error checking edge closure: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception checking edge closure: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception checking edge closure";
        ERR_PRINT(last_error);
    }
    
    return false;
}

Dictionary ocgd_topology_explorer::get_vertex_properties(int vertex_index) {
    Dictionary props;
    ERR_FAIL_COND_V_MSG(!has_shape(), props, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index, get_vertex_count(), props, "Vertex index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        
        TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(vertex_index + 1));
        
        props["index"] = vertex_index;
        props["position"] = get_vertex_position(vertex_index);
        props["tolerance"] = get_vertex_tolerance(vertex_index);
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting vertex properties: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting vertex properties: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting vertex properties";
        ERR_PRINT(last_error);
    }
    
    return props;
}

Vector3 ocgd_topology_explorer::get_vertex_position(int vertex_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index, get_vertex_count(), Vector3(), "Vertex index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        
        TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(vertex_index + 1));
        gp_Pnt point = BRep_Tool::Pnt(vertex);
        
        return Vector3(point.X(), point.Y(), point.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting vertex position: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting vertex position: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting vertex position";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

double ocgd_topology_explorer::measure_distance_point_to_edge(const Vector3& point, int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), -1.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), -1.0, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        gp_Pnt query_point(point.x, point.y, point.z);
        
        BRepExtrema_DistShapeShape dist_calc;
        BRepBuilderAPI_MakeVertex vertex_maker(query_point);
        dist_calc.LoadS1(vertex_maker.Vertex());
        dist_calc.LoadS2(edge);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            return dist_calc.Value();
        }
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring distance to edge: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring distance to edge: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring distance to edge";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_topology_explorer::measure_distance_face_to_face(int face_index1, int face_index2) {
    ERR_FAIL_COND_V_MSG(!has_shape(), -1.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index1, get_face_count(), -1.0, "Face index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(face_index2, get_face_count(), -1.0, "Face index 2 out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face1 = TopoDS::Face(face_map(face_index1 + 1));
        TopoDS_Face face2 = TopoDS::Face(face_map(face_index2 + 1));
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(face1);
        dist_calc.LoadS2(face2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            return dist_calc.Value();
        }
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring face to face distance: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring face to face distance: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring face to face distance";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_topology_explorer::measure_distance_edge_to_edge(int edge_index1, int edge_index2) {
    ERR_FAIL_COND_V_MSG(!has_shape(), -1.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index1, get_edge_count(), -1.0, "Edge index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(edge_index2, get_edge_count(), -1.0, "Edge index 2 out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge1 = TopoDS::Edge(edge_map(edge_index1 + 1));
        TopoDS_Edge edge2 = TopoDS::Edge(edge_map(edge_index2 + 1));
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(edge1);
        dist_calc.LoadS2(edge2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            return dist_calc.Value();
        }
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error measuring edge to edge distance: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception measuring edge to edge distance: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception measuring edge to edge distance";
        ERR_PRINT(last_error);
    }
    
    return -1.0;
}

double ocgd_topology_explorer::measure_distance_vertex_to_vertex(int vertex_index1, int vertex_index2) {
    ERR_FAIL_COND_V_MSG(!has_shape(), -1.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index1, get_vertex_count(), -1.0, "Vertex index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(vertex_index2, get_vertex_count(), -1.0, "Vertex index 2 out of range");
    
    Vector3 pos1 = get_vertex_position(vertex_index1);
    Vector3 pos2 = get_vertex_position(vertex_index2);
    
    return measure_distance_point_to_point(pos1, pos2);
}
Dictionary ocgd_topology_explorer::calculate_mass_properties() {
    Dictionary mass_props;
    ERR_FAIL_COND_V_MSG(!has_shape(), mass_props, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        
        // Calculate volume properties for solids
        GProp_GProps volume_props;
        BRepGProp::VolumeProperties(shape, volume_props);
        
        // Calculate surface properties
        GProp_GProps surface_props;
        BRepGProp::SurfaceProperties(shape, surface_props);
        
        mass_props["volume"] = volume_props.Mass();
        mass_props["surface_area"] = surface_props.Mass();
        
        gp_Pnt cog = volume_props.CentreOfMass();
        mass_props["center_of_gravity"] = Vector3(cog.X(), cog.Y(), cog.Z());
        
        // Moments of inertia
        gp_Mat inertia = volume_props.MatrixOfInertia();
        Dictionary inertia_dict;
        inertia_dict["xx"] = inertia(1,1);
        inertia_dict["xy"] = inertia(1,2);
        inertia_dict["xz"] = inertia(1,3);
        inertia_dict["yy"] = inertia(2,2);
        inertia_dict["yz"] = inertia(2,3);
        inertia_dict["zz"] = inertia(3,3);
        mass_props["moments_of_inertia"] = inertia_dict;
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating mass properties: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating mass properties: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating mass properties";
        ERR_PRINT(last_error);
    }
    
    return mass_props;
}

Vector3 ocgd_topology_explorer::get_center_of_gravity() {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        GProp_GProps props;
        BRepGProp::VolumeProperties(shape, props);
        
        gp_Pnt cog = props.CentreOfMass();
        return Vector3(cog.X(), cog.Y(), cog.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating center of gravity: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating center of gravity: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating center of gravity";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

double ocgd_topology_explorer::get_volume() {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        GProp_GProps props;
        BRepGProp::VolumeProperties(shape, props);
        return props.Mass();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating volume: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating volume: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating volume";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}

double ocgd_topology_explorer::get_surface_area() {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        GProp_GProps props;
        BRepGProp::SurfaceProperties(shape, props);
        return props.Mass();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating surface area: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating surface area: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating surface area";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}

// Additional missing method implementations
Array ocgd_topology_explorer::get_faces_of_solid(int solid_index) {
    Array faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), faces, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(solid_index, get_solid_count(), faces, "Solid index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape solid_map;
        TopExp::MapShapes(shape, TopAbs_SOLID, solid_map);
        
        TopoDS_Solid solid = TopoDS::Solid(solid_map(solid_index + 1));
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(solid, TopAbs_FACE, face_map);
        
        // Map back to global face indices
        TopTools_IndexedMapOfShape global_face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, global_face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            TopoDS_Face face = TopoDS::Face(face_map(i));
            int global_index = global_face_map.FindIndex(face);
            if (global_index > 0) {
                faces.append(global_index - 1);
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting faces of solid: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting faces of solid: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting faces of solid";
        ERR_PRINT(last_error);
    }
    
    return faces;
}

Array ocgd_topology_explorer::get_edges_of_face(int face_index) {
    Array edges;
    ERR_FAIL_COND_V_MSG(!has_shape(), edges, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), edges, "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(face, TopAbs_EDGE, edge_map);
        
        // Map back to global edge indices
        TopTools_IndexedMapOfShape global_edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, global_edge_map);
        
        for (int i = 1; i <= edge_map.Extent(); i++) {
            TopoDS_Edge edge = TopoDS::Edge(edge_map(i));
            int global_index = global_edge_map.FindIndex(edge);
            if (global_index > 0) {
                edges.append(global_index - 1);
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edges of face: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edges of face: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edges of face";
        ERR_PRINT(last_error);
    }
    
    return edges;
}

Array ocgd_topology_explorer::get_vertices_of_edge(int edge_index) {
    Array vertices;
    ERR_FAIL_COND_V_MSG(!has_shape(), vertices, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), vertices, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(edge, TopAbs_VERTEX, vertex_map);
        
        // Map back to global vertex indices
        TopTools_IndexedMapOfShape global_vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, global_vertex_map);
        
        for (int i = 1; i <= vertex_map.Extent(); i++) {
            TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(i));
            int global_index = global_vertex_map.FindIndex(vertex);
            if (global_index > 0) {
                vertices.append(global_index - 1);
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting vertices of edge: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting vertices of edge: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting vertices of edge";
        ERR_PRINT(last_error);
    }
    
    return vertices;
}

Array ocgd_topology_explorer::get_edges_of_vertex(int vertex_index) {
    Array edges;
    ERR_FAIL_COND_V_MSG(!has_shape(), edges, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index, get_vertex_count(), edges, "Vertex index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        
        TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(vertex_index + 1));
        
        // Build adjacency map
        TopTools_IndexedDataMapOfShapeListOfShape vertex_edge_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_VERTEX, TopAbs_EDGE, vertex_edge_map);
        
        if (vertex_edge_map.Contains(vertex)) {
            const TopTools_ListOfShape& edge_list = vertex_edge_map.FindFromKey(vertex);
            TopTools_IndexedMapOfShape global_edge_map;
            TopExp::MapShapes(shape, TopAbs_EDGE, global_edge_map);
            
            for (TopTools_ListIteratorOfListOfShape it(edge_list); it.More(); it.Next()) {
                TopoDS_Edge edge = TopoDS::Edge(it.Value());
                int global_index = global_edge_map.FindIndex(edge);
                if (global_index > 0) {
                    edges.append(global_index - 1);
                }
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edges of vertex: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edges of vertex: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edges of vertex";
        ERR_PRINT(last_error);
    }
    
    return edges;
}
Array ocgd_topology_explorer::get_adjacent_faces(int face_index) {
    Array adjacent_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), adjacent_faces, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), adjacent_faces, "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        
        // Build face-edge adjacency map
        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);
        
        // Get edges of this face
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(face, TopAbs_EDGE, edge_map);
        
        // For each edge, find adjacent faces
        for (int i = 1; i <= edge_map.Extent(); i++) {
            TopoDS_Edge edge = TopoDS::Edge(edge_map(i));
            if (edge_face_map.Contains(edge)) {
                const TopTools_ListOfShape& face_list = edge_face_map.FindFromKey(edge);
                for (TopTools_ListIteratorOfListOfShape it(face_list); it.More(); it.Next()) {
                    TopoDS_Face adj_face = TopoDS::Face(it.Value());
                    if (!adj_face.IsSame(face)) {
                        int global_index = face_map.FindIndex(adj_face);
                        if (global_index > 0) {
                            adjacent_faces.append(global_index - 1);
                        }
                    }
                }
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting adjacent faces: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting adjacent faces: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting adjacent faces";
        ERR_PRINT(last_error);
    }
    
    return adjacent_faces;
}

Array ocgd_topology_explorer::get_connected_edges(int edge_index) {
    Array connected_edges;
    ERR_FAIL_COND_V_MSG(!has_shape(), connected_edges, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), connected_edges, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        // Get vertices of this edge
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(edge, TopAbs_VERTEX, vertex_map);
        
        // Build vertex-edge adjacency map
        TopTools_IndexedDataMapOfShapeListOfShape vertex_edge_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_VERTEX, TopAbs_EDGE, vertex_edge_map);
        
        // For each vertex, find connected edges
        for (int i = 1; i <= vertex_map.Extent(); i++) {
            TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(i));
            if (vertex_edge_map.Contains(vertex)) {
                const TopTools_ListOfShape& edge_list = vertex_edge_map.FindFromKey(vertex);
                for (TopTools_ListIteratorOfListOfShape it(edge_list); it.More(); it.Next()) {
                    TopoDS_Edge conn_edge = TopoDS::Edge(it.Value());
                    if (!conn_edge.IsSame(edge)) {
                        int global_index = edge_map.FindIndex(conn_edge);
                        if (global_index > 0) {
                            connected_edges.append(global_index - 1);
                        }
                    }
                }
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting connected edges: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting connected edges: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting connected edges";
        ERR_PRINT(last_error);
    }
    
    return connected_edges;
}

Dictionary ocgd_topology_explorer::get_edge_bounds(int edge_index) {
    Dictionary bounds;
    ERR_FAIL_COND_V_MSG(!has_shape(), bounds, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), bounds, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        double first, last;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
        
        if (!curve.IsNull()) {
            bounds["first_parameter"] = first;
            bounds["last_parameter"] = last;
            bounds["range"] = last - first;
            
            gp_Pnt start_point = curve->Value(first);
            gp_Pnt end_point = curve->Value(last);
            
            bounds["start_point"] = Vector3(start_point.X(), start_point.Y(), start_point.Z());
            bounds["end_point"] = Vector3(end_point.X(), end_point.Y(), end_point.Z());
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge bounds: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge bounds: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge bounds";
        ERR_PRINT(last_error);
    }
    
    return bounds;
}

double ocgd_topology_explorer::get_edge_curvature_at_param(int edge_index, double parameter) {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), 0.0, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        double first, last;
        Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
        
        if (!curve.IsNull()) {
            double param = first + parameter * (last - first);
            ERR_FAIL_COND_V_MSG(param < first || param > last, 0.0, "Parameter out of range");
            
            GeomLProp_CLProps props(curve, param, 2, precision_tolerance);
            if (props.IsTangentDefined()) {
                return props.Curvature();
            }
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge curvature: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge curvature: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge curvature";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}

double ocgd_topology_explorer::get_vertex_tolerance(int vertex_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index, get_vertex_count(), 0.0, "Vertex index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape vertex_map;
        TopExp::MapShapes(shape, TopAbs_VERTEX, vertex_map);
        
        TopoDS_Vertex vertex = TopoDS::Vertex(vertex_map(vertex_index + 1));
        return BRep_Tool::Tolerance(vertex);
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting vertex tolerance: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting vertex tolerance: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting vertex tolerance";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}

Dictionary ocgd_topology_explorer::get_closest_points_face_to_face(int face_index1, int face_index2) {
    Dictionary result;
    ERR_FAIL_COND_V_MSG(!has_shape(), result, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index1, get_face_count(), result, "Face index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(face_index2, get_face_count(), result, "Face index 2 out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face1 = TopoDS::Face(face_map(face_index1 + 1));
        TopoDS_Face face2 = TopoDS::Face(face_map(face_index2 + 1));
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(face1);
        dist_calc.LoadS2(face2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            result["distance"] = dist_calc.Value();
            
            gp_Pnt p1 = dist_calc.PointOnShape1(1);
            gp_Pnt p2 = dist_calc.PointOnShape2(1);
            
            result["point1"] = Vector3(p1.X(), p1.Y(), p1.Z());
            result["point2"] = Vector3(p2.X(), p2.Y(), p2.Z());
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating face distances: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating face distances: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating face distances";
        ERR_PRINT(last_error);
    }
    
    return result;
}

Dictionary ocgd_topology_explorer::get_closest_points_edge_to_edge(int edge_index1, int edge_index2) {
    Dictionary result;
    ERR_FAIL_COND_V_MSG(!has_shape(), result, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index1, get_edge_count(), result, "Edge index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(edge_index2, get_edge_count(), result, "Edge index 2 out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge1 = TopoDS::Edge(edge_map(edge_index1 + 1));
        TopoDS_Edge edge2 = TopoDS::Edge(edge_map(edge_index2 + 1));
        
        BRepExtrema_DistShapeShape dist_calc;
        dist_calc.LoadS1(edge1);
        dist_calc.LoadS2(edge2);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            result["distance"] = dist_calc.Value();
            
            gp_Pnt p1 = dist_calc.PointOnShape1(1);
            gp_Pnt p2 = dist_calc.PointOnShape2(1);
            
            result["point1"] = Vector3(p1.X(), p1.Y(), p1.Z());
            result["point2"] = Vector3(p2.X(), p2.Y(), p2.Z());
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating edge distances: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating edge distances: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating edge distances";
        ERR_PRINT(last_error);
    }
    
    return result;
}

Vector3 ocgd_topology_explorer::get_closest_point_on_face(const Vector3& point, int face_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), Vector3(), "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        gp_Pnt query_point(point.x, point.y, point.z);
        
        BRepExtrema_DistShapeShape dist_calc;
        BRepBuilderAPI_MakeVertex vertex_maker(query_point);
        dist_calc.LoadS1(vertex_maker.Vertex());
        dist_calc.LoadS2(face);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            gp_Pnt closest = dist_calc.PointOnShape2(1);
            return Vector3(closest.X(), closest.Y(), closest.Z());
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding closest point on face: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding closest point on face: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding closest point on face";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

Vector3 ocgd_topology_explorer::get_closest_point_on_edge(const Vector3& point, int edge_index) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), Vector3(), "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        gp_Pnt query_point(point.x, point.y, point.z);
        
        BRepExtrema_DistShapeShape dist_calc;
        BRepBuilderAPI_MakeVertex vertex_maker(query_point);
        dist_calc.LoadS1(vertex_maker.Vertex());
        dist_calc.LoadS2(edge);
        dist_calc.Perform();
        
        if (dist_calc.IsDone() && dist_calc.NbSolution() > 0) {
            gp_Pnt closest = dist_calc.PointOnShape2(1);
            return Vector3(closest.X(), closest.Y(), closest.Z());
        }
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding closest point on edge: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding closest point on edge: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding closest point on edge";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

bool ocgd_topology_explorer::is_shape_manifold() {
    ERR_FAIL_COND_V_MSG(!has_shape(), false, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        
        // Build edge-face adjacency map
        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);
        
        // Check that each edge is shared by exactly 2 faces (manifold condition)
        for (int i = 1; i <= edge_face_map.Extent(); i++) {
            const TopTools_ListOfShape& face_list = edge_face_map(i);
            if (face_list.Extent() != 2) {
                return false; // Non-manifold edge found
            }
        }
        
        clear_error();
        return true;
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error checking manifold: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception checking manifold: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception checking manifold";
        ERR_PRINT(last_error);
    }
    
    return false;
}

Array ocgd_topology_explorer::find_free_edges() {
    Array free_edges;
    ERR_FAIL_COND_V_MSG(!has_shape(), free_edges, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        
        // Build edge-face adjacency map
        TopTools_IndexedDataMapOfShapeListOfShape edge_face_map;
        TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, edge_face_map);
        
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        // Find edges that belong to only one face (free edges)
        for (int i = 1; i <= edge_map.Extent(); i++) {
            TopoDS_Edge edge = TopoDS::Edge(edge_map(i));
            if (edge_face_map.Contains(edge)) {
                const TopTools_ListOfShape& face_list = edge_face_map.FindFromKey(edge);
                if (face_list.Extent() == 1) {
                    free_edges.append(i - 1); // Zero-based index
                }
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding free edges: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding free edges: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding free edges";
        ERR_PRINT(last_error);
    }
    
    return free_edges;
}

Array ocgd_topology_explorer::find_degenerate_faces() {
    Array degenerate_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), degenerate_faces, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            TopoDS_Face face = TopoDS::Face(face_map(i));
            
            // Check if face has zero area or other degenerate conditions
            GProp_GProps props;
            BRepGProp::SurfaceProperties(face, props);
            double area = props.Mass();
            
            if (area < precision_tolerance || area < 1e-10) {
                degenerate_faces.append(i - 1);
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding degenerate faces: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding degenerate faces: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding degenerate faces";
        ERR_PRINT(last_error);
    }
    
    return degenerate_faces;
}

Array ocgd_topology_explorer::find_small_features(double min_size) {
    Array small_features;
    ERR_FAIL_COND_V_MSG(!has_shape(), small_features, "No shape set for topology exploration");
    ERR_FAIL_COND_V_MSG(min_size <= 0.0, small_features, "Minimum size must be positive");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        
        // Check faces
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            TopoDS_Face face = TopoDS::Face(face_map(i));
            GProp_GProps props;
            BRepGProp::SurfaceProperties(face, props);
            double area = props.Mass();
            
            if (area < min_size) {
                Dictionary feature;
                feature["type"] = "face";
                feature["index"] = i - 1;
                feature["size"] = area;
                small_features.append(feature);
            }
        }
        
        // Check edges
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        for (int i = 1; i <= edge_map.Extent(); i++) {
            TopoDS_Edge edge = TopoDS::Edge(edge_map(i));
            GProp_GProps props;
            BRepGProp::LinearProperties(edge, props);
            double length = props.Mass();
            
            if (length < min_size) {
                Dictionary feature;
                feature["type"] = "edge";
                feature["index"] = i - 1;
                feature["size"] = length;
                small_features.append(feature);
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding small features: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding small features: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding small features";
        ERR_PRINT(last_error);
    }
    
    return small_features;
}

// Advanced face analysis implementation
Dictionary ocgd_topology_explorer::get_face_curvature_at_uv(int face_index, double u, double v) {
    Dictionary curvature;
    ERR_FAIL_COND_V_MSG(!has_shape(), curvature, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), curvature, "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        // Check UV bounds
        double u_min = surface.FirstUParameter();
        double u_max = surface.LastUParameter();
        double v_min = surface.FirstVParameter();
        double v_max = surface.LastVParameter();
        
        ERR_FAIL_COND_V_MSG(u < u_min || u > u_max, curvature, "U parameter out of surface bounds");
        ERR_FAIL_COND_V_MSG(v < v_min || v > v_max, curvature, "V parameter out of surface bounds");
        
        Handle(Geom_Surface) geom_surface = surface.Surface().Surface();
        GeomLProp_SLProps props(geom_surface, u, v, 2, precision_tolerance);
        
        if (props.IsCurvatureDefined()) {
            curvature["gaussian_curvature"] = props.GaussianCurvature();
            curvature["mean_curvature"] = props.MeanCurvature();
            curvature["max_curvature"] = props.MaxCurvature();
            curvature["min_curvature"] = props.MinCurvature();
            
            if (props.IsCurvatureDefined()) {
                gp_Dir max_dir, min_dir;
                props.CurvatureDirections(max_dir, min_dir);
                curvature["max_curvature_direction"] = Vector3(max_dir.X(), max_dir.Y(), max_dir.Z());
                curvature["min_curvature_direction"] = Vector3(min_dir.X(), min_dir.Y(), min_dir.Z());
            }
        }
        
        if (props.IsNormalDefined()) {
            gp_Dir normal = props.Normal();
            curvature["normal"] = Vector3(normal.X(), normal.Y(), normal.Z());
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error calculating face curvature: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception calculating face curvature: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception calculating face curvature";
        ERR_PRINT(last_error);
    }
    
    return curvature;
}

double ocgd_topology_explorer::get_face_gaussian_curvature_at_uv(int face_index, double u, double v) {
    Dictionary curvature = get_face_curvature_at_uv(face_index, u, v);
    if (curvature.has("gaussian_curvature")) {
        return curvature["gaussian_curvature"];
    }
    return 0.0;
}

double ocgd_topology_explorer::get_face_mean_curvature_at_uv(int face_index, double u, double v) {
    Dictionary curvature = get_face_curvature_at_uv(face_index, u, v);
    if (curvature.has("mean_curvature")) {
        return curvature["mean_curvature"];
    }
    return 0.0;
}

Dictionary ocgd_topology_explorer::get_edge_curvature_analysis(int edge_index) {
    Dictionary analysis;
    ERR_FAIL_COND_V_MSG(!has_shape(), analysis, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), analysis, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        BRepAdaptor_Curve curve(edge);
        
        double first = curve.FirstParameter();
        double last = curve.LastParameter();
        
        analysis["first_parameter"] = first;
        analysis["last_parameter"] = last;
        analysis["parameter_range"] = last - first;
        
        // Sample curvature at several points
        Array curvature_samples;
        int num_samples = 10;
        for (int i = 0; i <= num_samples; i++) {
            double param = first + (double(i) / num_samples) * (last - first);
            double curvature = get_edge_curvature_at_param(edge_index, (param - first) / (last - first));
            
            Dictionary sample;
            sample["parameter"] = param;
            sample["curvature"] = curvature;
            curvature_samples.append(sample);
        }
        analysis["curvature_samples"] = curvature_samples;
        
        // Calculate average and maximum curvature
        double max_curvature = 0.0;
        double avg_curvature = 0.0;
        for (int i = 0; i < curvature_samples.size(); i++) {
            Dictionary sample = curvature_samples[i];
            double curv = sample["curvature"];
            max_curvature = MAX(max_curvature, curv);
            avg_curvature += curv;
        }
        avg_curvature /= curvature_samples.size();
        
        analysis["max_curvature"] = max_curvature;
        analysis["average_curvature"] = avg_curvature;
        analysis["curve_type"] = get_edge_type(edge_index);
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error analyzing edge curvature: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception analyzing edge curvature: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception analyzing edge curvature";
        ERR_PRINT(last_error);
    }
    
    return analysis;
}

Dictionary ocgd_topology_explorer::get_face_uv_bounds(int face_index) {
    Dictionary bounds;
    ERR_FAIL_COND_V_MSG(!has_shape(), bounds, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), bounds, "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        bounds["u_min"] = surface.FirstUParameter();
        bounds["u_max"] = surface.LastUParameter();
        bounds["v_min"] = surface.FirstVParameter();
        bounds["v_max"] = surface.LastVParameter();
        bounds["u_range"] = surface.LastUParameter() - surface.FirstUParameter();
        bounds["v_range"] = surface.LastVParameter() - surface.FirstVParameter();
        bounds["is_u_closed"] = surface.IsUClosed();
        bounds["is_v_closed"] = surface.IsVClosed();
        bounds["is_u_periodic"] = surface.IsUPeriodic();
        bounds["is_v_periodic"] = surface.IsVPeriodic();
        
        if (surface.IsUPeriodic()) {
            bounds["u_period"] = surface.UPeriod();
        }
        if (surface.IsVPeriodic()) {
            bounds["v_period"] = surface.VPeriod();
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting face UV bounds: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting face UV bounds: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting face UV bounds";
        ERR_PRINT(last_error);
    }
    
    return bounds;
}

Vector3 ocgd_topology_explorer::evaluate_face_at_uv(int face_index, double u, double v) {
    ERR_FAIL_COND_V_MSG(!has_shape(), Vector3(), "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), Vector3(), "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        // Check bounds
        ERR_FAIL_COND_V_MSG(u < surface.FirstUParameter() || u > surface.LastUParameter(), 
                           Vector3(), "U parameter out of bounds");
        ERR_FAIL_COND_V_MSG(v < surface.FirstVParameter() || v > surface.LastVParameter(), 
                           Vector3(), "V parameter out of bounds");
        
        gp_Pnt point = surface.Value(u, v);
        return Vector3(point.X(), point.Y(), point.Z());
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error evaluating face at UV: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception evaluating face at UV: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception evaluating face at UV";
        ERR_PRINT(last_error);
    }
    
    return Vector3();
}

Dictionary ocgd_topology_explorer::get_face_derivatives_at_uv(int face_index, double u, double v) {
    Dictionary derivatives;
    ERR_FAIL_COND_V_MSG(!has_shape(), derivatives, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index, get_face_count(), derivatives, "Face index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        TopoDS_Face face = TopoDS::Face(face_map(face_index + 1));
        BRepAdaptor_Surface surface(face);
        
        Handle(Geom_Surface) geom_surface = surface.Surface().Surface();
        GeomLProp_SLProps props(geom_surface, u, v, 2, precision_tolerance);
        
        // Position
        if (props.IsNormalDefined()) {
            gp_Pnt point = props.Value();
            derivatives["position"] = Vector3(point.X(), point.Y(), point.Z());
        }
        
        // First derivatives
        if (props.D1U().Magnitude() > precision_tolerance) {
            gp_Vec d1u = props.D1U();
            derivatives["d1u"] = Vector3(d1u.X(), d1u.Y(), d1u.Z());
        }
        if (props.D1V().Magnitude() > precision_tolerance) {
            gp_Vec d1v = props.D1V();
            derivatives["d1v"] = Vector3(d1v.X(), d1v.Y(), d1v.Z());
        }
        
        // Second derivatives
        if (props.D2U().Magnitude() > precision_tolerance) {
            gp_Vec d2u = props.D2U();
            derivatives["d2u"] = Vector3(d2u.X(), d2u.Y(), d2u.Z());
        }
        if (props.D2V().Magnitude() > precision_tolerance) {
            gp_Vec d2v = props.D2V();
            derivatives["d2v"] = Vector3(d2v.X(), d2v.Y(), d2v.Z());
        }
        if (props.DUV().Magnitude() > precision_tolerance) {
            gp_Vec duv = props.DUV();
            derivatives["duv"] = Vector3(duv.X(), duv.Y(), duv.Z());
        }
        
        // Normal
        if (props.IsNormalDefined()) {
            gp_Dir normal = props.Normal();
            derivatives["normal"] = Vector3(normal.X(), normal.Y(), normal.Z());
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting face derivatives: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting face derivatives: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting face derivatives";
        ERR_PRINT(last_error);
    }
    
    return derivatives;
}

Array ocgd_topology_explorer::find_face_intersections(int face_index1, int face_index2) {
    Array intersections;
    ERR_FAIL_COND_V_MSG(!has_shape(), intersections, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(face_index1, get_face_count(), intersections, "Face index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(face_index2, get_face_count(), intersections, "Face index 2 out of range");
    
    try {
        // Get the faces using the existing mapping
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(current_shape->get_shape(), TopAbs_FACE, face_map);
        
        TopoDS_Face face1 = TopoDS::Face(face_map(face_index1 + 1));
        TopoDS_Face face2 = TopoDS::Face(face_map(face_index2 + 1));
        
        // Use BRepAlgoAPI_Section to find intersection
        BRepAlgoAPI_Section section(face1, face2);
        section.Build();
        
        if (section.IsDone()) {
            TopoDS_Shape intersection_shape = section.Shape();
            
            // Extract intersection curves/points
            TopExp_Explorer exp(intersection_shape, TopAbs_EDGE);
            while (exp.More()) {
                TopoDS_Edge edge = TopoDS::Edge(exp.Current());
                BRepAdaptor_Curve curve(edge);
                
                Dictionary intersection_info;
                intersection_info["type"] = "curve";
                intersection_info["start_point"] = Vector3(curve.Value(curve.FirstParameter()).X(),
                                                          curve.Value(curve.FirstParameter()).Y(),
                                                          curve.Value(curve.FirstParameter()).Z());
                intersection_info["end_point"] = Vector3(curve.Value(curve.LastParameter()).X(),
                                                        curve.Value(curve.LastParameter()).Y(),
                                                        curve.Value(curve.LastParameter()).Z());
                intersection_info["length"] = GCPnts_AbscissaPoint::Length(curve);
                
                intersections.append(intersection_info);
                exp.Next();
            }
        }
    } catch (const Standard_Failure& e) {
        ERR_PRINT("OpenCASCADE error during face intersection: " + String(e.GetMessageString()));
    } catch (...) {
        ERR_PRINT("Unknown error during face intersection calculation");
    }
    
    return intersections;
}

Array ocgd_topology_explorer::find_edge_intersections(int edge_index1, int edge_index2) {
    Array intersections;
    ERR_FAIL_COND_V_MSG(!has_shape(), intersections, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index1, get_edge_count(), intersections, "Edge index 1 out of range");
    ERR_FAIL_INDEX_V_MSG(edge_index2, get_edge_count(), intersections, "Edge index 2 out of range");
    
    try {
        // Get the edges using the existing mapping
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(current_shape->get_shape(), TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge1 = TopoDS::Edge(edge_map(edge_index1 + 1));
        TopoDS_Edge edge2 = TopoDS::Edge(edge_map(edge_index2 + 1));
        
        // Convert edges to curves
        BRepAdaptor_Curve curve1(edge1);
        BRepAdaptor_Curve curve2(edge2);
        
        // Use Extrema_ExtCC to find closest points/intersections
        Extrema_ExtCC extrema(curve1, curve2);
        
        if (extrema.IsDone() && extrema.NbExt() > 0) {
            for (int i = 1; i <= extrema.NbExt(); i++) {
                if (extrema.SquareDistance(i) < Precision::Confusion() * Precision::Confusion()) {
                    // Points are coincident - intersection found
                    Extrema_POnCurv point1, point2;
                    extrema.Points(i, point1, point2);
                    
                    gp_Pnt intersection_point = point1.Value();
                    
                    Dictionary intersection_info;
                    intersection_info["type"] = "point";
                    intersection_info["point"] = Vector3(intersection_point.X(), 
                                                        intersection_point.Y(), 
                                                        intersection_point.Z());
                    intersection_info["parameter1"] = point1.Parameter();
                    intersection_info["parameter2"] = point2.Parameter();
                    intersection_info["distance"] = sqrt(extrema.SquareDistance(i));
                    
                    intersections.append(intersection_info);
                }
            }
        }
    } catch (const Standard_Failure& e) {
        ERR_PRINT("OpenCASCADE error during edge intersection: " + String(e.GetMessageString()));
    } catch (...) {
        ERR_PRINT("Unknown error during edge intersection calculation");
    }
    
    return intersections;
}

bool ocgd_topology_explorer::faces_intersect(int face_index1, int face_index2) {
    Dictionary closest_points = get_closest_points_face_to_face(face_index1, face_index2);
    if (closest_points.has("distance")) {
        double distance = closest_points["distance"];
        return distance < precision_tolerance;
    }
    return false;
}

bool ocgd_topology_explorer::edges_intersect(int edge_index1, int edge_index2) {
    Dictionary closest_points = get_closest_points_edge_to_edge(edge_index1, edge_index2);
    if (closest_points.has("distance")) {
        double distance = closest_points["distance"];
        return distance < precision_tolerance;
    }
    return false;
}

Dictionary ocgd_topology_explorer::get_face_bounding_box(int face_index) {
    return get_face_bounds(face_index); // Reuse existing implementation
}

Dictionary ocgd_topology_explorer::get_edge_bounding_box(int edge_index) {
    Dictionary bounds;
    ERR_FAIL_COND_V_MSG(!has_shape(), bounds, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(edge_index, get_edge_count(), bounds, "Edge index out of range");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape edge_map;
        TopExp::MapShapes(shape, TopAbs_EDGE, edge_map);
        
        TopoDS_Edge edge = TopoDS::Edge(edge_map(edge_index + 1));
        
        Bnd_Box bbox;
        BRepBndLib::Add(edge, bbox);
        
        if (!bbox.IsVoid()) {
            double xmin, ymin, zmin, xmax, ymax, zmax;
            bbox.Get(xmin, ymin, zmin, xmax, ymax, zmax);
            
            bounds["min_x"] = xmin;
            bounds["min_y"] = ymin;
            bounds["min_z"] = zmin;
            bounds["max_x"] = xmax;
            bounds["max_y"] = ymax;
            bounds["max_z"] = zmax;
            bounds["size_x"] = xmax - xmin;
            bounds["size_y"] = ymax - ymin;
            bounds["size_z"] = zmax - zmin;
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error getting edge bounding box: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception getting edge bounding box: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception getting edge bounding box";
        ERR_PRINT(last_error);
    }
    
    return bounds;
}

Dictionary ocgd_topology_explorer::get_vertex_bounding_box(int vertex_index) {
    Dictionary bounds;
    ERR_FAIL_COND_V_MSG(!has_shape(), bounds, "No shape set for topology exploration");
    ERR_FAIL_INDEX_V_MSG(vertex_index, get_vertex_count(), bounds, "Vertex index out of range");
    
    try {
        Vector3 pos = get_vertex_position(vertex_index);
        double tolerance = get_vertex_tolerance(vertex_index);
        
        bounds["min_x"] = pos.x - tolerance;
        bounds["min_y"] = pos.y - tolerance;
        bounds["min_z"] = pos.z - tolerance;
        bounds["max_x"] = pos.x + tolerance;
        bounds["max_y"] = pos.y + tolerance;
        bounds["max_z"] = pos.z + tolerance;
        bounds["size_x"] = 2.0 * tolerance;
        bounds["size_y"] = 2.0 * tolerance;
        bounds["size_z"] = 2.0 * tolerance;
        bounds["center"] = pos;
        bounds["tolerance"] = tolerance;
        
        clear_error();
    } catch (...) {
        last_error = "Error getting vertex bounding box";
        ERR_PRINT(last_error);
    }
    
    return bounds;
}

Dictionary ocgd_topology_explorer::get_moments_of_inertia() {
    Dictionary mass_props = calculate_mass_properties();
    if (mass_props.has("moments_of_inertia")) {
        return mass_props["moments_of_inertia"];
    }
    return Dictionary();
}

// Feature recognition implementations
Array ocgd_topology_explorer::find_holes() {
    Array holes;
    ERR_FAIL_COND_V_MSG(!has_shape(), holes, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            TopoDS_Face face = TopoDS::Face(face_map(i));
            String face_type = get_face_type(i - 1);
            
            // Look for cylindrical faces that could be holes
            if (face_type == "cylinder") {
                BRepAdaptor_Surface surface(face);
                gp_Cylinder cylinder = surface.Cylinder();
                
                Dictionary hole;
                hole["face_index"] = i - 1;
                hole["type"] = "cylindrical_hole";
                hole["radius"] = cylinder.Radius();
                
                gp_Pnt center = cylinder.Location();
                hole["center"] = Vector3(center.X(), center.Y(), center.Z());
                
                gp_Dir axis = cylinder.Axis().Direction();
                hole["axis"] = Vector3(axis.X(), axis.Y(), axis.Z());
                
                holes.append(hole);
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding holes: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding holes: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding holes";
        ERR_PRINT(last_error);
    }
    
    return holes;
}

Array ocgd_topology_explorer::find_fillets() {
    Array fillets;
    ERR_FAIL_COND_V_MSG(!has_shape(), fillets, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            TopoDS_Face face = TopoDS::Face(face_map(i));
            String face_type = get_face_type(i - 1);
            
            // Look for cylindrical or toroidal faces that could be fillets
            if (face_type == "cylinder" || face_type == "torus") {
                double area = get_face_area(i - 1);
                
                // Small cylindrical/toroidal faces are likely fillets
                if (area < 100.0) { // Threshold for fillet detection
                    Dictionary fillet;
                    fillet["face_index"] = i - 1;
                    fillet["type"] = face_type + "_fillet";
                    fillet["area"] = area;
                    
                    if (face_type == "cylinder") {
                        BRepAdaptor_Surface surface(face);
                        gp_Cylinder cylinder = surface.Cylinder();
                        fillet["radius"] = cylinder.Radius();
                    }
                    
                    fillets.append(fillet);
                }
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding fillets: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding fillets: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding fillets";
        ERR_PRINT(last_error);
    }
    
    return fillets;
}

Array ocgd_topology_explorer::find_chamfers() {
    Array chamfers;
    ERR_FAIL_COND_V_MSG(!has_shape(), chamfers, "No shape set for topology exploration");
    
    try {
        TopoDS_Shape shape = current_shape->get_shape();
        TopTools_IndexedMapOfShape face_map;
        TopExp::MapShapes(shape, TopAbs_FACE, face_map);
        
        for (int i = 1; i <= face_map.Extent(); i++) {
            String face_type = get_face_type(i - 1);
            
            // Look for small planar faces that could be chamfers
            if (face_type == "plane") {
                double area = get_face_area(i - 1);
                
                // Small planar faces might be chamfers
                if (area < 50.0) { // Threshold for chamfer detection
                    Dictionary chamfer;
                    chamfer["face_index"] = i - 1;
                    chamfer["type"] = "planar_chamfer";
                    chamfer["area"] = area;
                    chamfer["normal"] = get_face_normal(i - 1);
                    
                    chamfers.append(chamfer);
                }
            }
        }
        
        clear_error();
    } catch (const Standard_Failure& e) {
        last_error = String("OpenCASCADE error finding chamfers: ") + e.GetMessageString();
        ERR_PRINT(last_error);
    } catch (const std::exception& e) {
        last_error = String("Standard exception finding chamfers: ") + e.what();
        ERR_PRINT(last_error);
    } catch (...) {
        last_error = "Unknown exception finding chamfers";
        ERR_PRINT(last_error);
    }
    
    return chamfers;
}

Array ocgd_topology_explorer::find_cylindrical_faces() {
    Array cylindrical_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), cylindrical_faces, "No shape set for topology exploration");
    
    try {
        for (int i = 0; i < get_face_count(); i++) {
            if (get_face_type(i) == "cylinder") {
                cylindrical_faces.append(i);
            }
        }
        clear_error();
    } catch (...) {
        last_error = "Error finding cylindrical faces";
        ERR_PRINT(last_error);
    }
    
    return cylindrical_faces;
}

Array ocgd_topology_explorer::find_planar_faces() {
    Array planar_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), planar_faces, "No shape set for topology exploration");
    
    try {
        for (int i = 0; i < get_face_count(); i++) {
            if (get_face_type(i) == "plane") {
                planar_faces.append(i);
            }
        }
        clear_error();
    } catch (...) {
        last_error = "Error finding planar faces";
        ERR_PRINT(last_error);
    }
    
    return planar_faces;
}

Array ocgd_topology_explorer::find_spherical_faces() {
    Array spherical_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), spherical_faces, "No shape set for topology exploration");
    
    try {
        for (int i = 0; i < get_face_count(); i++) {
            if (get_face_type(i) == "sphere") {
                spherical_faces.append(i);
            }
        }
        clear_error();
    } catch (...) {
        last_error = "Error finding spherical faces";
        ERR_PRINT(last_error);
    }
    
    return spherical_faces;
}

Array ocgd_topology_explorer::find_conical_faces() {
    Array conical_faces;
    ERR_FAIL_COND_V_MSG(!has_shape(), conical_faces, "No shape set for topology exploration");
    
    try {
        for (int i = 0; i < get_face_count(); i++) {
            if (get_face_type(i) == "cone") {
                conical_faces.append(i);
            }
        }
        clear_error();
    } catch (...) {
        last_error = "Error finding conical faces";
        ERR_PRINT(last_error);
    }
    
    return conical_faces;
}

// Analysis and export methods
void ocgd_topology_explorer::set_cache_results(bool cache) { 
    cache_results = cache; 
}

bool ocgd_topology_explorer::get_cache_results() const { 
    return cache_results; 
}

Dictionary ocgd_topology_explorer::export_topology_info() {
    Dictionary info;
    ERR_FAIL_COND_V_MSG(!has_shape(), info, "No shape set for topology exploration");
    
    try {
        info["shape_statistics"] = get_shape_statistics();
        info["mass_properties"] = calculate_mass_properties();
        info["bounding_box"] = get_overall_bounding_box();
        info["is_valid"] = is_shape_valid();
        info["is_closed"] = is_shape_closed();
        info["is_manifold"] = is_shape_manifold();
        info["free_edges"] = find_free_edges();
        info["degenerate_faces"] = find_degenerate_faces();
        info["holes"] = find_holes();
        info["fillets"] = find_fillets();
        info["chamfers"] = find_chamfers();
        info["cylindrical_faces"] = find_cylindrical_faces();
        info["planar_faces"] = find_planar_faces();
        info["spherical_faces"] = find_spherical_faces();
        info["conical_faces"] = find_conical_faces();
        
        clear_error();
    } catch (...) {
        last_error = "Error exporting topology info";
        ERR_PRINT(last_error);
    }
    
    return info;
}

String ocgd_topology_explorer::export_topology_report() {
    ERR_FAIL_COND_V_MSG(!has_shape(), "", "No shape set for topology exploration");
    
    String report = "=== TOPOLOGY ANALYSIS REPORT ===\n\n";
    
    try {
        Dictionary stats = get_shape_statistics();
        report += "Shape Statistics:\n";
        report += String("  Faces: ") + String::num(int(stats["face_count"])) + "\n";
        report += String("  Edges: ") + String::num(int(stats["edge_count"])) + "\n";
        report += String("  Vertices: ") + String::num(int(stats["vertex_count"])) + "\n";
        report += String("  Solids: ") + String::num(int(stats["solid_count"])) + "\n";
        report += String("  Shells: ") + String::num(int(stats["shell_count"])) + "\n";
        report += String("  Wires: ") + String::num(int(stats["wire_count"])) + "\n\n";
        
        report += "Shape Validation:\n";
        report += String("  Valid: ") + (is_shape_valid() ? "Yes" : "No") + "\n";
        report += String("  Closed: ") + (is_shape_closed() ? "Yes" : "No") + "\n";
        report += String("  Manifold: ") + (is_shape_manifold() ? "Yes" : "No") + "\n\n";
        
        Dictionary mass_props = calculate_mass_properties();
        if (mass_props.has("volume")) {
            report += "Mass Properties:\n";
            report += String("  Volume: ") + String::num(double(mass_props["volume"])) + "\n";
            report += String("  Surface Area: ") + String::num(double(mass_props["surface_area"])) + "\n";
            Vector3 cog = mass_props["center_of_gravity"];
            report += String("  Center of Gravity: (") + String::num(cog.x) + ", " + String::num(cog.y) + ", " + String::num(cog.z) + ")\n\n";
        }
        
        Array free_edges = find_free_edges();
        if (free_edges.size() > 0) {
            report += String("Free Edges: ") + String::num(free_edges.size()) + " found\n";
        }
        
        Array holes = find_holes();
        if (holes.size() > 0) {
            report += String("Holes: ") + String::num(holes.size()) + " detected\n";
        }
        
        Array fillets = find_fillets();
        if (fillets.size() > 0) {
            report += String("Fillets: ") + String::num(fillets.size()) + " detected\n";
        }
        
        Array chamfers = find_chamfers();
        if (chamfers.size() > 0) {
            report += String("Chamfers: ") + String::num(chamfers.size()) + " detected\n";
        }
        
        clear_error();
    } catch (...) {
        last_error = "Error generating topology report";
        ERR_PRINT(last_error);
    }
    
    return report;
}

bool ocgd_topology_explorer::save_topology_analysis(const String& file_path) {
    ERR_FAIL_COND_V_MSG(file_path.is_empty(), false, "File path is empty");
    
    String report = export_topology_report();
    
    // This would require file I/O implementation
    // For now, just print to console
    UtilityFunctions::print(report);
    
    return !report.is_empty();
}

bool ocgd_topology_explorer::compare_topologies(const Ref<ocgd_shape>& other_shape) {
    ERR_FAIL_COND_V_MSG(!has_shape(), false, "No current shape set for comparison");
    ERR_FAIL_NULL_V_MSG(other_shape.ptr(), false, "Other shape is null");
    
    Dictionary diff = get_topology_differences(other_shape);
    return diff.is_empty();
}

Dictionary ocgd_topology_explorer::get_topology_differences(const Ref<ocgd_shape>& other_shape) {
    Dictionary differences;
    ERR_FAIL_COND_V_MSG(!has_shape(), differences, "No current shape set for comparison");
    ERR_FAIL_NULL_V_MSG(other_shape.ptr(), differences, "Other shape is null");
    
    try {
        // Create temporary explorer for other shape
        Ref<ocgd_topology_explorer> other_explorer = ocgd_topology_explorer::new_explorer();
        other_explorer->set_shape(other_shape);
        
        Dictionary current_stats = get_shape_statistics();
        Dictionary other_stats = other_explorer->get_shape_statistics();
        
        differences["face_count_diff"] = int(current_stats["face_count"]) - int(other_stats["face_count"]);
        differences["edge_count_diff"] = int(current_stats["edge_count"]) - int(other_stats["edge_count"]);
        differences["vertex_count_diff"] = int(current_stats["vertex_count"]) - int(other_stats["vertex_count"]);
        differences["solid_count_diff"] = int(current_stats["solid_count"]) - int(other_stats["solid_count"]);
        
        double current_volume = get_volume();
        double other_volume = other_explorer->get_volume();
        differences["volume_diff"] = current_volume - other_volume;
        
        double current_area = get_surface_area();
        double other_area = other_explorer->get_surface_area();
        differences["surface_area_diff"] = current_area - other_area;
        
        clear_error();
    } catch (...) {
        last_error = "Error comparing topologies";
        ERR_PRINT(last_error);
    }
    
    return differences;
}

double ocgd_topology_explorer::calculate_shape_similarity(const Ref<ocgd_shape>& other_shape) {
    ERR_FAIL_COND_V_MSG(!has_shape(), 0.0, "No current shape set for comparison");
    ERR_FAIL_NULL_V_MSG(other_shape.ptr(), 0.0, "Other shape is null");
    
    try {
        Dictionary differences = get_topology_differences(other_shape);
        
        // Simple similarity metric based on topology differences
        double similarity = 1.0;
        
        int face_diff = abs(int(differences["face_count_diff"]));
        int edge_diff = abs(int(differences["edge_count_diff"]));
        int vertex_diff = abs(int(differences["vertex_count_diff"]));
        
        double total_current = get_face_count() + get_edge_count() + get_vertex_count();
        double total_diff = face_diff + edge_diff + vertex_diff;
        
        if (total_current > 0) {
            similarity = 1.0 - (total_diff / total_current);
        }
        
        similarity = MAX(0.0, MIN(1.0, similarity));
        return similarity;
    } catch (...) {
        last_error = "Error calculating shape similarity";
        ERR_PRINT(last_error);
    }
    
    return 0.0;
}