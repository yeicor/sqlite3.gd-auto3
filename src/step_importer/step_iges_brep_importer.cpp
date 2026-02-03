#include "step_iges_brep_importer.h"

#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <Poly_Triangulation.hxx>
#include <BRepTools.hxx>
#include <filesystem>
#include <Standard_TypeDef.hxx>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

StepIgesBRepImporter::StepIgesBRepImporter() = default;

StepIgesBRepImporter::~StepIgesBRepImporter() = default;

void StepIgesBRepImporter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("import", "String"), &StepIgesBRepImporter::import);
}


Error StepIgesBRepImporter::import(const String& p_source_file) const
{
    TopoDS_Shape shape;
    // TODO: Clean way of importing multiple shapes at once (STEP files)
    // TODO: Colors and other STEP features should be supported (export as GLTF/GLB to reuse OpenCASCADE and Godot code)
    bool is_step = p_source_file.ends_with(".step") || p_source_file.ends_with(".stp");
    bool is_iges = p_source_file.ends_with(".iges") || p_source_file.ends_with(".igs");
    if (is_step || is_iges)
    {
        IFSelect_ReturnStatus status;
        XSControl_Reader reader;
        if (is_step)
        {
            STEPControl_Reader m_reader;
            status = reader.ReadFile(p_source_file.utf8().get_data());
            reader = static_cast<XSControl_Reader>(m_reader);
        }
        if (is_iges)
        {
            IGESControl_Reader m_reader;
            status = reader.ReadFile(p_source_file.utf8().get_data());
            reader = static_cast<XSControl_Reader>(m_reader);
        }
        if (status != IFSelect_RetDone)
        {
            ERR_PRINT("Failed to read STEP file.");
            return ERR_FILE_CANT_OPEN;
        }
        if (!reader.TransferRoots())
        {
            ERR_PRINT("Failed to transfer STEP roots.");
            return ERR_CANT_CREATE;
        }
        shape = reader.OneShape();
    }
    else if (p_source_file.ends_with(".brep"))
    {
        BRep_Builder builder;
        Standard_Boolean result = BRepTools::Read(shape, p_source_file.utf8().get_data(), builder);
        if (!result)
        {
            ERR_PRINT("Failed to read BREP file.");
            return ERR_FILE_CANT_OPEN;
        }
    }
    else
    {
        ERR_PRINT("Unsupported file format. Only STEP and BREP files are supported.");
        return ERR_UNAVAILABLE;
    }

    double linear_deflection = 0.01;
    double angular_deflection = 0.1;
    BRepMesh_IncrementalMesh mesher(shape, linear_deflection, false, angular_deflection, true);

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    for (TopExp_Explorer faceExp(shape, TopAbs_FACE); faceExp.More(); faceExp.Next())
    {
        TopoDS_Face face = TopoDS::Face(faceExp.Current());
        TopLoc_Location the_location;
        // Grab the location of the mesh/face?
        face.Location(the_location);
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, the_location);

        if (triangulation.IsNull())
            continue;

        PackedVector3Array vertices;
        PackedInt32Array indices;

        for (int i = 1; i <= triangulation->NbNodes(); ++i)
        {
            gp_Pnt p = triangulation->Node(i);
            vertices.push_back(Vector3(p.X(), p.Y(), p.Z()));
        }

        for (int i = 1; i <= triangulation->NbTriangles(); ++i)
        {
            int n1, n2, n3;
            triangulation->Triangle(i).Get(n1, n2, n3);
            if (face.Orientation() != TopAbs_REVERSED)
            {
                indices.push_back(n1 - 1);
                indices.push_back(n2 - 1);
                indices.push_back(n3 - 1);
            }
            else
            {
                indices.push_back(n3 - 1);
                indices.push_back(n2 - 1);
                indices.push_back(n1 - 1);
            }
        }

        Array arrays;
        arrays.resize(Mesh::ARRAY_MAX);
        arrays[Mesh::ARRAY_VERTEX] = vertices;
        arrays[Mesh::ARRAY_INDEX] = indices;

        mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    }

    String save_path_with_ext = p_source_file.get_base_dir() + "/" + p_source_file.get_file().get_basename() + ".res";
    ResourceSaver* saver = ResourceSaver::get_singleton();
    return saver->save(mesh, save_path_with_ext);
}
