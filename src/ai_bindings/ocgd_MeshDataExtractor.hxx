#ifndef _ocgd_MeshDataExtractor_HeaderFile
#define _ocgd_MeshDataExtractor_HeaderFile

/**
 * ocgd_MeshDataExtractor.hxx
 *
 * Godot GDExtension wrapper for extracting triangulation data from OpenCASCADE shapes.
 *
 * This utility class provides methods to extract vertices, triangles, normals, and UV coordinates
 * from triangulated OpenCASCADE shapes, converting them into Godot-friendly data structures
 * that can be used to create Godot meshes.
 *
 * The class handles the extraction of:
 * - 3D vertex positions
 * - Triangle indices
 * - Surface normals (when available)
 * - UV texture coordinates (when available)
 * - Per-face material and color information
 *
 * Original OCCT headers: <opencascade/BRep_Tool.hxx>, <opencascade/Poly_Triangulation.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/Poly_Array1OfTriangle.hxx>
#include <opencascade/TColgp_Array1OfPnt.hxx>
#include <opencascade/TColgp_Array1OfPnt2d.hxx>
#include <opencascade/TShort_Array1OfShortReal.hxx>
#include <opencascade/Poly_MeshPurpose.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/gp_Trsf.hxx>
#include <opencascade/BRepMesh_IncrementalMesh.hxx>
#include <opencascade/TopExp_Explorer.hxx>

#include <opencascade/gp_Vec3f.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_MeshDataExtractor
 *
 * Utility class to extract triangulation data from OpenCASCADE shapes and convert
 * it into Godot-compatible mesh data structures.
 *
 * This class provides methods to:
 * - Extract vertices, triangles, normals from triangulated faces
 * - Convert OpenCASCADE data to Godot PackedArrays
 * - Handle face-by-face extraction for multi-material meshes
 * - Apply transformations and locations
 * - Extract UV coordinates for texture mapping
 */
class ocgd_MeshDataExtractor : public RefCounted {
    GDCLASS(ocgd_MeshDataExtractor, RefCounted);

protected:
    static void _bind_methods();

private:
    // Current extraction settings
    bool _include_normals;
    bool _include_uvs;
    bool _merge_vertices;
    double _vertex_merge_tolerance;

public:
    //! Default constructor
    ocgd_MeshDataExtractor();

    //! Destructor
    virtual ~ocgd_MeshDataExtractor();

    //! Set whether to include normal vectors in extraction
    void set_include_normals(bool include);

    //! Get whether normal vectors are included
    bool get_include_normals() const;

    //! Set whether to include UV coordinates in extraction
    void set_include_uvs(bool include);

    //! Get whether UV coordinates are included
    bool get_include_uvs() const;

    //! Set whether to merge duplicate vertices
    void set_merge_vertices(bool merge);

    //! Get whether vertices are merged
    bool get_merge_vertices() const;

    //! Set tolerance for vertex merging
    void set_vertex_merge_tolerance(double tolerance);

    //! Get vertex merging tolerance
    double get_vertex_merge_tolerance() const;

    //! Extract complete mesh data from a shape
    //! Returns a Dictionary with keys: "vertices", "triangles", "normals", "uvs"
    Dictionary extract_mesh_data(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection = 0.1, double angular_deflection = 0.1, bool compute_normals = true);

    //! Extract mesh data from a specific face
    //! Returns a Dictionary with mesh data for the single face
    Dictionary extract_face_data(const Ref<ocgd_TopoDS_Shape>& face, double linear_deflection = 0.1, double angular_deflection = 0.1, bool compute_normals = true);

    //! Extract just vertices from a shape
    PackedVector3Array extract_vertices(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Extract just triangle indices from a shape
    PackedInt32Array extract_triangles(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Extract just normals from a shape
    PackedVector3Array extract_normals(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Extract just UV coordinates from a shape
    PackedVector2Array extract_uvs(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Extract mesh data per face (useful for multi-material meshes)
    //! Returns an Array of Dictionaries, one per face
    Array extract_per_face_data(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection = 0.1, double angular_deflection = 0.1, bool compute_normals = true);

    //! Check if a shape has triangulation data
    bool has_triangulation(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Check if a specific face has triangulation data
    bool face_has_triangulation(const Ref<ocgd_TopoDS_Shape>& face) const;

    //! Get triangulation statistics for a shape
    //! Returns Dictionary with "face_count", "triangle_count", "vertex_count"
    Dictionary get_triangulation_stats(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Get bounding box of triangulated geometry
    //! Returns Dictionary with "min" and "max" Vector3 values
    Dictionary get_triangulation_bounds(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Extract surface area from triangulation
    double get_triangulated_area(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Get detailed triangulation information for debugging
    //! Returns Dictionary with detailed info about each face's triangulation
    Dictionary get_detailed_triangulation_info(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Debug validation method that provides detailed information about validation failures
    //! Returns Dictionary with detailed validation results for each face
    Dictionary debug_validate_triangulation(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Ensure shape has triangulation, applying it if needed
    //! Returns true if triangulation is available after the operation
    bool ensure_triangulation(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection, double angular_deflection, bool compute_normals = false) const;

    //! Compute normals for existing triangulation
    //! Returns true if normals were computed successfully
    bool compute_normals(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Mesh purpose enumeration for different triangulation types
    enum MeshPurpose {
        MESH_PURPOSE_NONE = Poly_MeshPurpose_NONE,
        MESH_PURPOSE_USER = Poly_MeshPurpose_USER,
        MESH_PURPOSE_PRESENTATION = Poly_MeshPurpose_Presentation,
        MESH_PURPOSE_CALCULATION = Poly_MeshPurpose_Calculation,
        MESH_PURPOSE_ACTIVE = Poly_MeshPurpose_Active,
        MESH_PURPOSE_LOADED = Poly_MeshPurpose_Loaded,
        MESH_PURPOSE_ANY_FALLBACK = Poly_MeshPurpose_AnyFallback
    };

    //! Extract mesh data with specific mesh purpose
    Dictionary extract_mesh_data_with_purpose(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection = 0.1, double angular_deflection = 0.1, int purpose = 0, bool compute_normals = true);

private:
    //! Internal helper to extract triangulation from a single face
    Handle(Poly_Triangulation) get_face_triangulation(const TopoDS_Face& face, int purpose = Poly_MeshPurpose_NONE) const;

    //! Internal helper to validate triangulation data integrity
    bool validate_triangulation(const Handle(Poly_Triangulation)& triangulation) const;

    //! Internal helper to convert OpenCASCADE triangulation to Godot arrays
    Dictionary convert_triangulation_to_dict(const Handle(Poly_Triangulation)& triangulation,
                                            const TopLoc_Location& location = TopLoc_Location(),
                                            const TopoDS_Face* face = nullptr) const;

    //! Internal helper to apply transformation to vertices
    void apply_transformation(PackedVector3Array& vertices, const gp_Trsf& transformation) const;

    //! Internal helper to merge duplicate vertices
    void merge_duplicate_vertices(PackedVector3Array& vertices, PackedInt32Array& triangles,
                                 PackedVector3Array& normals, PackedVector2Array& uvs) const;

    //! Internal helper to compute surface area from triangulation
    double compute_triangulated_area(const Handle(Poly_Triangulation)& triangulation,
                                   const TopLoc_Location& location = TopLoc_Location()) const;
};

VARIANT_ENUM_CAST(ocgd_MeshDataExtractor::MeshPurpose);

#endif // _ocgd_MeshDataExtractor_HeaderFile
