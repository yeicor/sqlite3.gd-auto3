#include "OCCMeshExtractor.hxx"
#include "OCCShape.hxx"
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <Poly_Triangulation.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <TopoDS.hxx>
#include <godot_cpp/core/class_db.hpp>

godot::TypedArray<godot::Dictionary> OCCMeshExtractor::extract_mesh(const godot::Ref<OCCShape> &shape, double deflection) {
    godot::TypedArray<godot::Dictionary> mesh_arrays;
    if (shape.is_null() || shape->is_null())
        return mesh_arrays;

    BRepMesh_IncrementalMesh(shape->get_occ_shape(), deflection);

    TopExp_Explorer exp;
    for (exp.Init(shape->get_occ_shape(), TopAbs_FACE); exp.More(); exp.Next()) {
        TopoDS_Face face = TopoDS::Face(exp.Current());
        TopLoc_Location location;
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);
        if (triangulation.IsNull()) continue;

        Standard_Integer nNodes = triangulation->NbNodes();
        godot::PackedVector3Array godot_vertices;
        for (Standard_Integer i = 1; i <= nNodes; ++i) {
            gp_Pnt p = triangulation->Node(i);
            godot_vertices.push_back({(float)p.X(), (float)p.Y(), (float)p.Z()});
        }

        Standard_Integer nTriangles = triangulation->NbTriangles();
        godot::PackedInt32Array godot_indices;
        for (Standard_Integer i = 1; i <= nTriangles; ++i) {
            Poly_Triangle tri = triangulation->Triangle(i);
            Standard_Integer n1, n2, n3;
            tri.Get(n1, n2, n3);
            // Convert to zero-based indexing for Godot:
            godot_indices.push_back(n1 - 1);
            godot_indices.push_back(n2 - 1);
            godot_indices.push_back(n3 - 1);
        }

        godot::Dictionary face_dict;
        face_dict["vertices"] = godot_vertices;
        face_dict["indices"] = godot_indices;
        if (triangulation->HasNormals()) {
            godot::PackedVector3Array godot_normals;
            for (Standard_Integer i = 1; i <= nNodes; ++i) {
                gp_Dir normal = triangulation->Normal(i);
                godot_normals.push_back({(float)normal.X(), (float)normal.Y(), (float)normal.Z()});
            }
            face_dict["normals"] = godot_normals;
        }
        mesh_arrays.push_back(face_dict);
    }
    return mesh_arrays;
}

void OCCMeshExtractor::_bind_methods() {
    godot::ClassDB::bind_static_method("OCCMeshExtractor", godot::D_METHOD("extract_mesh", "shape", "deflection"), &OCCMeshExtractor::extract_mesh, 0.01);
}
