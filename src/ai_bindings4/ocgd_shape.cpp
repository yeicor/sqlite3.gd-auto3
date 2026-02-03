#include "ocgd_shape.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// OpenCASCADE includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Compound.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ExtPF.hxx>
#include <TopLoc_Location.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <IGESCAFControl_Writer.hxx>
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <STEPCAFControl_Writer.hxx>

using namespace godot;

ocgd_shape::ocgd_shape() {
    occ_shape = nullptr;
    owns_shape = false;
}

ocgd_shape::~ocgd_shape() {
    clear_shape();
}

void ocgd_shape::_bind_methods() {
    ClassDB::bind_static_method("ocgd_shape", D_METHOD("new_shape"), &ocgd_shape::new_shape);

    
    ClassDB::bind_method(D_METHOD("has_shape"), &ocgd_shape::has_shape);
    ClassDB::bind_method(D_METHOD("clear_shape"), &ocgd_shape::clear_shape);
    
    ClassDB::bind_method(D_METHOD("get_shape_type"), &ocgd_shape::get_shape_type);
    ClassDB::bind_method(D_METHOD("get_num_faces"), &ocgd_shape::get_num_faces);
    ClassDB::bind_method(D_METHOD("get_num_edges"), &ocgd_shape::get_num_edges);
    ClassDB::bind_method(D_METHOD("get_num_vertices"), &ocgd_shape::get_num_vertices);
    
    ClassDB::bind_method(D_METHOD("get_volume"), &ocgd_shape::get_volume);
    ClassDB::bind_method(D_METHOD("get_surface_area"), &ocgd_shape::get_surface_area);
    ClassDB::bind_method(D_METHOD("get_center_of_mass"), &ocgd_shape::get_center_of_mass);
    ClassDB::bind_method(D_METHOD("get_bounding_box"), &ocgd_shape::get_bounding_box);
    
    ClassDB::bind_method(D_METHOD("is_valid"), &ocgd_shape::is_valid);
    ClassDB::bind_method(D_METHOD("is_closed"), &ocgd_shape::is_closed);
    ClassDB::bind_method(D_METHOD("is_infinite"), &ocgd_shape::is_infinite);
    
    ClassDB::bind_method(D_METHOD("copy"), &ocgd_shape::copy);
    ClassDB::bind_method(D_METHOD("transform_translate", "translation"), &ocgd_shape::transform_translate);
    ClassDB::bind_method(D_METHOD("transform_rotate", "axis", "angle"), &ocgd_shape::transform_rotate);
    ClassDB::bind_method(D_METHOD("transform_scale", "factor"), &ocgd_shape::transform_scale);
    ClassDB::bind_method(D_METHOD("transform_scale_xyz", "scale"), &ocgd_shape::transform_scale_xyz);
    
    ClassDB::bind_method(D_METHOD("get_faces"), &ocgd_shape::get_faces);
    ClassDB::bind_method(D_METHOD("get_edges"), &ocgd_shape::get_edges);
    ClassDB::bind_method(D_METHOD("get_vertices"), &ocgd_shape::get_vertices);
    ClassDB::bind_method(D_METHOD("get_subshapes", "shape_type"), &ocgd_shape::get_subshapes);
    
    ClassDB::bind_method(D_METHOD("distance_to_shape", "other"), &ocgd_shape::distance_to_shape);
    ClassDB::bind_method(D_METHOD("distance_to_point", "point"), &ocgd_shape::distance_to_point);
    ClassDB::bind_method(D_METHOD("closest_point_to", "point"), &ocgd_shape::closest_point_to);
    
    ClassDB::bind_method(D_METHOD("fuse_with", "other"), &ocgd_shape::fuse_with);
    ClassDB::bind_method(D_METHOD("cut_with", "other"), &ocgd_shape::cut_with);
    ClassDB::bind_method(D_METHOD("common_with", "other"), &ocgd_shape::common_with);
    
    ClassDB::bind_method(D_METHOD("export_step", "file_path"), &ocgd_shape::export_step);
    ClassDB::bind_method(D_METHOD("export_iges", "file_path"), &ocgd_shape::export_iges);
    ClassDB::bind_method(D_METHOD("export_brep", "file_path"), &ocgd_shape::export_brep);
    ClassDB::bind_method(D_METHOD("export_stl", "file_path"), &ocgd_shape::export_stl);
    
    ClassDB::bind_method(D_METHOD("extract_mesh_data"), &ocgd_shape::extract_mesh_data);
    ClassDB::bind_method(D_METHOD("extract_triangulated_mesh"), &ocgd_shape::extract_triangulated_mesh);
    
    ClassDB::bind_method(D_METHOD("get_type"), &ocgd_shape::get_type);
    ClassDB::bind_method(D_METHOD("is_null"), &ocgd_shape::is_null);
    ClassDB::bind_method(D_METHOD("get_shape_info"), &ocgd_shape::get_shape_info);
    
    ClassDB::bind_method(D_METHOD("get_hash_code"), &ocgd_shape::get_hash_code);
    ClassDB::bind_method(D_METHOD("is_same_as", "other"), &ocgd_shape::is_same_as);
    ClassDB::bind_method(D_METHOD("is_equal_to", "other"), &ocgd_shape::is_equal_to);
}

Ref<ocgd_shape> ocgd_shape::new_shape() {
    return memnew(ocgd_shape);
}



void ocgd_shape::set_shape(const TopoDS_Shape& shape) {
    clear_shape();
    occ_shape = new TopoDS_Shape(shape);
    owns_shape = true;
}

TopoDS_Shape ocgd_shape::get_shape() const {
    if (occ_shape) {
        return *occ_shape;
    }
    return TopoDS_Shape();
}

bool ocgd_shape::has_shape() const {
    return occ_shape != nullptr && !occ_shape->IsNull();
}

void ocgd_shape::clear_shape() {
    if (occ_shape && owns_shape) {
        delete occ_shape;
    }
    occ_shape = nullptr;
    owns_shape = false;
}

String ocgd_shape::get_shape_type() const {
    if (!has_shape()) {
        return "NULL";
    }
    
    switch (occ_shape->ShapeType()) {
        case TopAbs_COMPOUND: return "COMPOUND";
        case TopAbs_COMPSOLID: return "COMPSOLID";
        case TopAbs_SOLID: return "SOLID";
        case TopAbs_SHELL: return "SHELL";
        case TopAbs_FACE: return "FACE";
        case TopAbs_WIRE: return "WIRE";
        case TopAbs_EDGE: return "EDGE";
        case TopAbs_VERTEX: return "VERTEX";
        default: return "UNKNOWN";
    }
}

int ocgd_shape::get_num_faces() const {
    if (!has_shape()) return 0;
    
    int count = 0;
    for (TopExp_Explorer exp(*occ_shape, TopAbs_FACE); exp.More(); exp.Next()) {
        count++;
    }
    return count;
}

int ocgd_shape::get_num_edges() const {
    if (!has_shape()) return 0;
    
    int count = 0;
    for (TopExp_Explorer exp(*occ_shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        count++;
    }
    return count;
}

int ocgd_shape::get_num_vertices() const {
    if (!has_shape()) return 0;
    
    int count = 0;
    for (TopExp_Explorer exp(*occ_shape, TopAbs_VERTEX); exp.More(); exp.Next()) {
        count++;
    }
    return count;
}

double ocgd_shape::get_volume() const {
    if (!has_shape()) return 0.0;
    
    try {
        GProp_GProps props;
        BRepGProp::VolumeProperties(*occ_shape, props);
        return props.Mass();
    } catch (...) {
        return 0.0;
    }
}

double ocgd_shape::get_surface_area() const {
    if (!has_shape()) return 0.0;
    
    try {
        GProp_GProps props;
        BRepGProp::SurfaceProperties(*occ_shape, props);
        return props.Mass();
    } catch (...) {
        return 0.0;
    }
}

Vector3 ocgd_shape::get_center_of_mass() const {
    if (!has_shape()) return Vector3();
    
    try {
        GProp_GProps props;
        BRepGProp::VolumeProperties(*occ_shape, props);
        gp_Pnt center = props.CentreOfMass();
        return Vector3(center.X(), center.Y(), center.Z());
    } catch (...) {
        return Vector3();
    }
}

Array ocgd_shape::get_bounding_box() const {
    Array bbox;
    if (!has_shape()) {
        bbox.append(Vector3());
        bbox.append(Vector3());
        return bbox;
    }
    
    try {
        Bnd_Box box;
        BRepBndLib::Add(*occ_shape, box);
        
        double xmin, ymin, zmin, xmax, ymax, zmax;
        box.Get(xmin, ymin, zmin, xmax, ymax, zmax);
        
        bbox.append(Vector3(xmin, ymin, zmin));
        bbox.append(Vector3(xmax, ymax, zmax));
    } catch (...) {
        bbox.append(Vector3());
        bbox.append(Vector3());
    }
    
    return bbox;
}

bool ocgd_shape::is_valid() const {
    if (!has_shape()) return false;
    
    try {
        BRepCheck_Analyzer analyzer(*occ_shape);
        return analyzer.IsValid();
    } catch (...) {
        return false;
    }
}

bool ocgd_shape::is_closed() const {
    if (!has_shape()) return false;
    return occ_shape->Closed();
}

bool ocgd_shape::is_infinite() const {
    if (!has_shape()) return false;
    return occ_shape->Infinite();
}

Ref<ocgd_shape> ocgd_shape::copy() const {
    if (!has_shape()) {
        return new_shape();
    }
    
    Ref<ocgd_shape> copy_shape = new_shape();
    copy_shape->_set_shape_internal(*occ_shape);
    return copy_shape;
}

void ocgd_shape::transform_translate(const Vector3& translation) {
    if (!has_shape()) return;
    
    try {
        gp_Trsf trsf;
        trsf.SetTranslation(gp_Vec(translation.x, translation.y, translation.z));
        
        BRepBuilderAPI_Transform transformer(*occ_shape, trsf);
        if (transformer.IsDone()) {
            set_shape(transformer.Shape());
        }
    } catch (...) {
        // Transform failed
    }
}

void ocgd_shape::transform_rotate(const Vector3& axis, double angle) {
    if (!has_shape()) return;
    
    try {
        gp_Ax1 rotation_axis(gp_Pnt(0, 0, 0), gp_Dir(axis.x, axis.y, axis.z));
        gp_Trsf trsf;
        trsf.SetRotation(rotation_axis, angle);
        
        BRepBuilderAPI_Transform transformer(*occ_shape, trsf);
        if (transformer.IsDone()) {
            set_shape(transformer.Shape());
        }
    } catch (...) {
        // Transform failed
    }
}

void ocgd_shape::transform_scale(double factor) {
    if (!has_shape()) return;
    
    try {
        gp_Trsf trsf;
        trsf.SetScale(gp_Pnt(0, 0, 0), factor);
        
        BRepBuilderAPI_Transform transformer(*occ_shape, trsf);
        if (transformer.IsDone()) {
            set_shape(transformer.Shape());
        }
    } catch (...) {
        // Transform failed
    }
}

void ocgd_shape::transform_scale_xyz(const Vector3& scale) {
    if (!has_shape()) return;
    
    // OpenCASCADE doesn't support non-uniform scaling directly
    // This would require more complex transformation
    double avg_scale = (scale.x + scale.y + scale.z) / 3.0;
    transform_scale(avg_scale);
}

Array ocgd_shape::get_faces() const {
    Array faces;
    if (!has_shape()) return faces;
    
    for (TopExp_Explorer exp(*occ_shape, TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        Ref<ocgd_shape> face_wrapper = new_shape();
        face_wrapper->_set_shape_internal(face);
        faces.append(face_wrapper);
    }
    
    return faces;
}

Array ocgd_shape::get_edges() const {
    Array edges;
    if (!has_shape()) return edges;
    
    for (TopExp_Explorer exp(*occ_shape, TopAbs_EDGE); exp.More(); exp.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(exp.Current());
        Ref<ocgd_shape> edge_wrapper = new_shape();
        edge_wrapper->_set_shape_internal(edge);
        edges.append(edge_wrapper);
    }
    
    return edges;
}

Array ocgd_shape::get_vertices() const {
    Array vertices;
    if (!has_shape()) return vertices;
    
    for (TopExp_Explorer exp(*occ_shape, TopAbs_VERTEX); exp.More(); exp.Next()) {
        TopoDS_Vertex vertex = TopoDS::Vertex(exp.Current());
        Ref<ocgd_shape> vertex_wrapper = new_shape();
        vertex_wrapper->_set_shape_internal(vertex);
        vertices.append(vertex_wrapper);
    }
    
    return vertices;
}

Array ocgd_shape::get_subshapes(const String& shape_type) const {
    Array subshapes;
    if (!has_shape()) return subshapes;
    
    TopAbs_ShapeEnum type_enum;
    String type_upper = shape_type.to_upper();
    
    if (type_upper == "FACE") type_enum = TopAbs_FACE;
    else if (type_upper == "EDGE") type_enum = TopAbs_EDGE;
    else if (type_upper == "VERTEX") type_enum = TopAbs_VERTEX;
    else if (type_upper == "WIRE") type_enum = TopAbs_WIRE;
    else if (type_upper == "SHELL") type_enum = TopAbs_SHELL;
    else if (type_upper == "SOLID") type_enum = TopAbs_SOLID;
    else if (type_upper == "COMPOUND") type_enum = TopAbs_COMPOUND;
    else if (type_upper == "COMPSOLID") type_enum = TopAbs_COMPSOLID;
    else return subshapes;
    
    for (TopExp_Explorer exp(*occ_shape, type_enum); exp.More(); exp.Next()) {
        Ref<ocgd_shape> subshape_wrapper = new_shape();
        subshape_wrapper->_set_shape_internal(exp.Current());
        subshapes.append(subshape_wrapper);
    }
    
    return subshapes;
}

double ocgd_shape::distance_to_shape(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return -1.0;
    }
    
    try {
        BRepExtrema_DistShapeShape dist(*occ_shape, other->get_shape());
        if (dist.IsDone()) {
            return dist.Value();
        }
    } catch (...) {
        // Distance calculation failed
    }
    
    return -1.0;
}

double ocgd_shape::distance_to_point(const Vector3& point) const {
    if (!has_shape()) return -1.0;
    
    try {
        gp_Pnt pnt(point.x, point.y, point.z);
        BRepExtrema_DistShapeShape dist(*occ_shape, BRepBuilderAPI_MakeVertex(pnt));
        if (dist.IsDone()) {
            return dist.Value();
        }
    } catch (...) {
        // Distance calculation failed
    }
    
    return -1.0;
}

Vector3 ocgd_shape::closest_point_to(const Vector3& point) const {
    if (!has_shape()) return Vector3();
    
    try {
        gp_Pnt pnt(point.x, point.y, point.z);
        BRepExtrema_DistShapeShape dist(*occ_shape, BRepBuilderAPI_MakeVertex(pnt));
        if (dist.IsDone() && dist.NbSolution() > 0) {
            gp_Pnt closest = dist.PointOnShape1(1);
            return Vector3(closest.X(), closest.Y(), closest.Z());
        }
    } catch (...) {
        // Closest point calculation failed
    }
    
    return Vector3();
}

Ref<ocgd_shape> ocgd_shape::fuse_with(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return new_shape();
    }
    
    try {
        BRepAlgoAPI_Fuse fuser(*occ_shape, other->get_shape());
        if (fuser.IsDone()) {
            Ref<ocgd_shape> result = new_shape();
            result->_set_shape_internal(fuser.Shape());
            return result;
        }
    } catch (...) {
        // Fuse operation failed
    }
    
    return new_shape();
}

Ref<ocgd_shape> ocgd_shape::cut_with(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return new_shape();
    }
    
    try {
        BRepAlgoAPI_Cut cutter(*occ_shape, other->get_shape());
        if (cutter.IsDone()) {
            Ref<ocgd_shape> result = new_shape();
            result->_set_shape_internal(cutter.Shape());
            return result;
        }
    } catch (...) {
        // Cut operation failed
    }
    
    return new_shape();
}

Ref<ocgd_shape> ocgd_shape::common_with(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return new_shape();
    }
    
    try {
        BRepAlgoAPI_Common commoner(*occ_shape, other->get_shape());
        if (commoner.IsDone()) {
            Ref<ocgd_shape> result = new_shape();
            result->_set_shape_internal(commoner.Shape());
            return result;
        }
    } catch (...) {
        // Common operation failed
    }
    
    return new_shape();
}

bool ocgd_shape::export_step(const String& file_path) const {
    if (!has_shape()) return false;
    
    try {
        // Create XCAF document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get shape tool and add shape
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
        TDF_Label label = shape_tool->AddShape(*occ_shape);
        
        // Create writer and write
        STEPCAFControl_Writer writer;
        if (writer.Transfer(doc, STEPControl_AsIs)) {
            CharString path_utf8 = file_path.utf8();
            IFSelect_ReturnStatus status = writer.Write(path_utf8.get_data());
            return status == IFSelect_RetDone;
        }
    } catch (...) {
        // Export failed
    }
    
    return false;
}

bool ocgd_shape::export_iges(const String& file_path) const {
    if (!has_shape()) return false;
    
    try {
        // Create XCAF document
        Handle(TDocStd_Document) doc;
        Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
        app->NewDocument("MDTV-XCAF", doc);
        
        // Get shape tool and add shape
        Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
        TDF_Label label = shape_tool->AddShape(*occ_shape);
        
        // Create writer and write
        IGESCAFControl_Writer writer;
        if (writer.Transfer(doc)) {
            CharString path_utf8 = file_path.utf8();
            Standard_Boolean status = writer.Write(path_utf8.get_data());
            return status == Standard_True;
        }
    } catch (...) {
        // Export failed
    }
    
    return false;
}

bool ocgd_shape::export_brep(const String& file_path) const {
    if (!has_shape()) return false;
    
    try {
        CharString path_utf8 = file_path.utf8();
        return BRepTools::Write(*occ_shape, path_utf8.get_data());
    } catch (...) {
        return false;
    }
}

bool ocgd_shape::export_stl(const String& file_path) const {
    if (!has_shape()) return false;
    
    try {
        // Mesh the shape first
        BRepMesh_IncrementalMesh mesher(*occ_shape, 0.1);
        
        StlAPI_Writer writer;
        CharString path_utf8 = file_path.utf8();
        return writer.Write(*occ_shape, path_utf8.get_data());
    } catch (...) {
        return false;
    }
}

Dictionary ocgd_shape::extract_mesh_data() const {
    Dictionary mesh_data;
    if (!has_shape()) return mesh_data;
    
    try {
        // Mesh the shape
        BRepMesh_IncrementalMesh mesher(*occ_shape, 0.1);
        
        Array vertices;
        Array normals;
        Array indices;
        
        // Extract triangulation from each face
        for (TopExp_Explorer exp(*occ_shape, TopAbs_FACE); exp.More(); exp.Next()) {
            TopoDS_Face face = TopoDS::Face(exp.Current());
            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
            
            if (!triangulation.IsNull()) {
                int nb_nodes = triangulation->NbNodes();
                int nb_triangles = triangulation->NbTriangles();
                
                int vertex_offset = vertices.size() / 3;
                
                // Add vertices
                for (int i = 1; i <= nb_nodes; i++) {
                    gp_Pnt pnt = triangulation->Node(i).Transformed(location);
                    vertices.append(pnt.X());
                    vertices.append(pnt.Y());
                    vertices.append(pnt.Z());
                }
                
                // Add triangles
                for (int i = 1; i <= nb_triangles; i++) {
                    const Poly_Triangle& triangle = triangulation->Triangle(i);
                    int n1, n2, n3;
                    triangle.Get(n1, n2, n3);
                    
                    indices.append(vertex_offset + n1 - 1);
                    indices.append(vertex_offset + n2 - 1);
                    indices.append(vertex_offset + n3 - 1);
                }
            }
        }
        
        // Calculate normals for each triangle
        PackedFloat32Array calculated_normals;
        calculated_normals.resize(vertices.size());
        
        // Initialize all normals to zero
        for (int i = 0; i < calculated_normals.size(); i++) {
            calculated_normals[i] = 0.0f;
        }
        
        // Calculate face normals and accumulate vertex normals
        for (int i = 0; i < indices.size(); i += 3) {
            int i1 = int(indices[i]) * 3;
            int i2 = int(indices[i + 1]) * 3;
            int i3 = int(indices[i + 2]) * 3;
            
            // Get triangle vertices
            Vector3 v1(vertices[i1], vertices[i1 + 1], vertices[i1 + 2]);
            Vector3 v2(vertices[i2], vertices[i2 + 1], vertices[i2 + 2]);
            Vector3 v3(vertices[i3], vertices[i3 + 1], vertices[i3 + 2]);
            
            // Calculate face normal using cross product
            Vector3 edge1 = v2 - v1;
            Vector3 edge2 = v3 - v1;
            Vector3 face_normal = edge1.cross(edge2).normalized();
            
            // Accumulate normal for each vertex of the triangle
            calculated_normals[i1] += face_normal.x;
            calculated_normals[i1 + 1] += face_normal.y;
            calculated_normals[i1 + 2] += face_normal.z;
            
            calculated_normals[i2] += face_normal.x;
            calculated_normals[i2 + 1] += face_normal.y;
            calculated_normals[i2 + 2] += face_normal.z;
            
            calculated_normals[i3] += face_normal.x;
            calculated_normals[i3 + 1] += face_normal.y;
            calculated_normals[i3 + 2] += face_normal.z;
        }
        
        // Normalize all vertex normals
        for (int i = 0; i < calculated_normals.size(); i += 3) {
            Vector3 normal(calculated_normals[i], calculated_normals[i + 1], calculated_normals[i + 2]);
            normal = normal.normalized();
            calculated_normals[i] = normal.x;
            calculated_normals[i + 1] = normal.y;
            calculated_normals[i + 2] = normal.z;
        }

        mesh_data["vertices"] = vertices;
        mesh_data["indices"] = indices;
        mesh_data["normals"] = calculated_normals;
        
    } catch (...) {
        // Mesh extraction failed
    }
    
    return mesh_data;
}

Array ocgd_shape::extract_triangulated_mesh() const {
    Array triangles;
    if (!has_shape()) return triangles;
    
    try {
        // Mesh the shape
        BRepMesh_IncrementalMesh mesher(*occ_shape, 0.1);
        
        // Extract triangulation from each face
        for (TopExp_Explorer exp(*occ_shape, TopAbs_FACE); exp.More(); exp.Next()) {
            TopoDS_Face face = TopoDS::Face(exp.Current());
            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
            
            if (!triangulation.IsNull()) {
                int nb_triangles = triangulation->NbTriangles();
                
                for (int i = 1; i <= nb_triangles; i++) {
                    const Poly_Triangle& triangle = triangulation->Triangle(i);
                    int n1, n2, n3;
                    triangle.Get(n1, n2, n3);
                    
                    Array tri;
                    gp_Pnt p1 = triangulation->Node(n1).Transformed(location);
                    gp_Pnt p2 = triangulation->Node(n2).Transformed(location);
                    gp_Pnt p3 = triangulation->Node(n3).Transformed(location);
                    
                    tri.append(Vector3(p1.X(), p1.Y(), p1.Z()));
                    tri.append(Vector3(p2.X(), p2.Y(), p2.Z()));
                    tri.append(Vector3(p3.X(), p3.Y(), p3.Z()));
                    
                    triangles.append(tri);
                }
            }
        }
        
    } catch (...) {
        // Mesh extraction failed
    }
    
    return triangles;
}

String ocgd_shape::get_type() const {
    return "ocgd_shape";
}

bool ocgd_shape::is_null() const {
    return !has_shape();
}

Dictionary ocgd_shape::get_shape_info() const {
    Dictionary info;
    info["type"] = get_type();
    info["shape_type"] = get_shape_type();
    info["has_shape"] = has_shape();
    info["is_valid"] = is_valid();
    info["is_closed"] = is_closed();
    info["is_infinite"] = is_infinite();
    info["num_faces"] = get_num_faces();
    info["num_edges"] = get_num_edges();
    info["num_vertices"] = get_num_vertices();
    info["volume"] = get_volume();
    info["surface_area"] = get_surface_area();
    info["center_of_mass"] = get_center_of_mass();
    info["bounding_box"] = get_bounding_box();
    return info;
}

int ocgd_shape::get_hash_code() const {
    if (!has_shape()) return 0;
    return std::hash<TopoDS_Shape>{}(*occ_shape);
}

bool ocgd_shape::is_same_as(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return false;
    }
    return occ_shape->IsSame(other->get_shape());
}

bool ocgd_shape::is_equal_to(const Ref<ocgd_shape>& other) const {
    if (!has_shape() || !other.is_valid() || !other->has_shape()) {
        return false;
    }
    return occ_shape->IsEqual(other->get_shape());
}

void ocgd_shape::_set_shape_internal(const TopoDS_Shape& shape, bool take_ownership) {
    clear_shape();
    if (take_ownership) {
        occ_shape = new TopoDS_Shape(shape);
        owns_shape = true;
    } else {
        occ_shape = const_cast<TopoDS_Shape*>(&shape);
        owns_shape = false;
    }
}

const TopoDS_Shape* ocgd_shape::_get_shape_ptr() const {
    return occ_shape;
}