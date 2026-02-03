/**
 * ocgd_MeshDataExtractor.cpp
 *
 * Godot GDExtension wrapper implementation for extracting triangulation data from OpenCASCADE shapes.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_MeshDataExtractor.hxx"
#include "ocgd_EnhancedNormals.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/TopoDS.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopAbs.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/Poly_Triangle.hxx>
#include <opencascade/TColgp_Array1OfPnt.hxx>
#include <opencascade/TColgp_Array1OfPnt2d.hxx>
#include <opencascade/TShort_Array1OfShortReal.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Pnt2d.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/gp_Trsf.hxx>
#include <opencascade/Bnd_Box.hxx>
#include <opencascade/BRepBndLib.hxx>
#include <opencascade/TopoDS.hxx>

#include <vector>

using namespace godot;

ocgd_MeshDataExtractor::ocgd_MeshDataExtractor() {
    _include_normals = true;
    _include_uvs = true;
    _merge_vertices = false;
    _vertex_merge_tolerance = 1e-6;
}

ocgd_MeshDataExtractor::~ocgd_MeshDataExtractor() {
    // No cleanup needed
}

void ocgd_MeshDataExtractor::set_include_normals(bool include) {
    _include_normals = include;
}

bool ocgd_MeshDataExtractor::get_include_normals() const {
    return _include_normals;
}

void ocgd_MeshDataExtractor::set_include_uvs(bool include) {
    _include_uvs = include;
}

bool ocgd_MeshDataExtractor::get_include_uvs() const {
    return _include_uvs;
}

void ocgd_MeshDataExtractor::set_merge_vertices(bool merge) {
    _merge_vertices = merge;
}

bool ocgd_MeshDataExtractor::get_merge_vertices() const {
    return _merge_vertices;
}

void ocgd_MeshDataExtractor::set_vertex_merge_tolerance(double tolerance) {
    _vertex_merge_tolerance = tolerance;
}

double ocgd_MeshDataExtractor::get_vertex_merge_tolerance() const {
    return _vertex_merge_tolerance;
}

Dictionary ocgd_MeshDataExtractor::extract_mesh_data(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection, double angular_deflection, bool compute_normals) {
    return extract_mesh_data_with_purpose(shape, linear_deflection, angular_deflection, MESH_PURPOSE_NONE, compute_normals);
}

Dictionary ocgd_MeshDataExtractor::extract_mesh_data_with_purpose(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection, double angular_deflection, int purpose, bool compute_normals) {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract mesh data - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract mesh data - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract mesh data - OpenCASCADE shape is null");
            return result;
        }

        PackedVector3Array all_vertices;
        PackedInt32Array all_triangles;
        PackedVector3Array all_normals;
        PackedVector2Array all_uvs;

        // Ensure triangulation with normal computation if requested
        ensure_triangulation(shape, linear_deflection, angular_deflection, compute_normals);

        // Explore all faces in the shape
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int vertex_offset = 0;

        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            try {
                // Get triangulation for this face
                Handle(Poly_Triangulation) triangulation = get_face_triangulation(face, purpose);
                if (triangulation.IsNull()) {
                    face_explorer.Next();
                    continue;
                }

                // Use validation but be more lenient - only skip if severely corrupted
                if (!validate_triangulation(triangulation)) {
                    UtilityFunctions::printerr("MeshDataExtractor: Skipping face with severely corrupted triangulation");
                    face_explorer.Next();
                    continue;
                }

                // Get face location
                TopLoc_Location face_location;
                BRep_Tool::Triangulation(face, face_location);

                // Convert triangulation to Godot arrays
                Dictionary face_data = convert_triangulation_to_dict(triangulation, face_location, &face);

                if (face_data.has("vertices")) {
                    PackedVector3Array face_vertices = face_data["vertices"];
                    PackedInt32Array face_triangles = face_data["triangles"];

                    // Append vertices
                    for (int i = 0; i < face_vertices.size(); i++) {
                        all_vertices.append(face_vertices[i]);
                    }

                    // Append triangles with vertex offset
                    for (int i = 0; i < face_triangles.size(); i++) {
                        all_triangles.append(face_triangles[i] + vertex_offset);
                    }

                    // Append normals if available and requested
                    if (_include_normals && face_data.has("normals")) {
                        PackedVector3Array face_normals = face_data["normals"];
                        for (int i = 0; i < face_normals.size(); i++) {
                            all_normals.append(face_normals[i]);
                        }
                    }

                    // Append UVs if available and requested
                    if (_include_uvs && face_data.has("uvs")) {
                        PackedVector2Array face_uvs = face_data["uvs"];
                        for (int i = 0; i < face_uvs.size(); i++) {
                            all_uvs.append(face_uvs[i]);
                        }
                    }

                    vertex_offset += face_vertices.size();
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception processing face - " + String(e.GetMessageString()));
                // Continue with next face
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception processing face - " + String(e.what()));
                // Continue with next face
            }

            face_explorer.Next();
        }

        // Apply vertex merging if requested
        if (_merge_vertices && all_vertices.size() > 0) {
            try {
                merge_duplicate_vertices(all_vertices, all_triangles, all_normals, all_uvs);
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception merging vertices - " + String(e.GetMessageString()));
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception merging vertices - " + String(e.what()));
            }
        }

        // Build result dictionary
        result["vertices"] = all_vertices;
        result["triangles"] = all_triangles;

        if (_include_normals && all_normals.size() > 0) {
            result["normals"] = all_normals;
        }

        if (_include_uvs && all_uvs.size() > 0) {
            result["uvs"] = all_uvs;
        }

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting mesh data - " + String(e.GetMessageString()));
        return Dictionary();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting mesh data - " + String(e.what()));
        return Dictionary();
    }
}

Dictionary ocgd_MeshDataExtractor::extract_face_data(const Ref<ocgd_TopoDS_Shape>& face, double linear_deflection, double angular_deflection, bool compute_normals) {
    Dictionary result;

    if (face.is_null() || face->is_null()) {
        UtilityFunctions::printerr("MeshDataExtractor: Cannot extract from null face");
        return result;
    }

    const TopoDS_Shape& occt_shape = face->get_occt_shape();
    if (occt_shape.ShapeType() != TopAbs_FACE) {
        UtilityFunctions::printerr("MeshDataExtractor: Shape is not a face");
        return result;
    }

    const TopoDS_Face& occt_face = TopoDS::Face(occt_shape);

    // Ensure triangulation with normal computation if requested
    ensure_triangulation(face, linear_deflection, angular_deflection, compute_normals);

    TopLoc_Location location;
    Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(occt_face, location);

    if (triangulation.IsNull()) {
        UtilityFunctions::printerr("MeshDataExtractor: Face has no triangulation");
        return result;
    }

    return convert_triangulation_to_dict(triangulation, location, &occt_face);
}

PackedVector3Array ocgd_MeshDataExtractor::extract_vertices(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        Dictionary mesh_data = extract_mesh_data(shape);
        if (mesh_data.has("vertices")) {
            return mesh_data["vertices"];
        }
        return PackedVector3Array();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting vertices - " + String(e.GetMessageString()));
        return PackedVector3Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting vertices - " + String(e.what()));
        return PackedVector3Array();
    }
}

PackedInt32Array ocgd_MeshDataExtractor::extract_triangles(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        Dictionary mesh_data = extract_mesh_data(shape);
        if (mesh_data.has("triangles")) {
            return mesh_data["triangles"];
        }
        return PackedInt32Array();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting triangles - " + String(e.GetMessageString()));
        return PackedInt32Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting triangles - " + String(e.what()));
        return PackedInt32Array();
    }
}

PackedVector3Array ocgd_MeshDataExtractor::extract_normals(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        Dictionary mesh_data = extract_mesh_data(shape);
        if (mesh_data.has("normals")) {
            return mesh_data["normals"];
        }
        return PackedVector3Array();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting normals - " + String(e.GetMessageString()));
        return PackedVector3Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting normals - " + String(e.what()));
        return PackedVector3Array();
    }
}

PackedVector2Array ocgd_MeshDataExtractor::extract_uvs(const Ref<ocgd_TopoDS_Shape>& shape) {
    try {
        Dictionary mesh_data = extract_mesh_data(shape);
        if (mesh_data.has("uvs")) {
            return mesh_data["uvs"];
        }
        return PackedVector2Array();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting UVs - " + String(e.GetMessageString()));
        return PackedVector2Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting UVs - " + String(e.what()));
        return PackedVector2Array();
    }
}

Array ocgd_MeshDataExtractor::extract_per_face_data(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection, double angular_deflection, bool compute_normals) {
    Array result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract per-face data - shape reference is null");
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract per-face data - shape is null");
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot extract per-face data - OpenCASCADE shape is null");
            return result;
        }

        // Ensure triangulation with normal computation if requested
        ensure_triangulation(shape, linear_deflection, angular_deflection, compute_normals);

        // Explore all faces in the shape
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        int face_index = 0;

        while (face_explorer.More()) {
            try {
                const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

                // Create a shape wrapper for this face
                Ref<ocgd_TopoDS_Shape> face_shape = memnew(ocgd_TopoDS_Shape);
                face_shape->set_occt_shape(face);

                // Extract mesh data for this face
                Dictionary face_data = extract_face_data(face_shape);
                face_data["face_index"] = face_index;

                result.append(face_data);
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception processing face " + String::num(face_index) + " - " + String(e.GetMessageString()));
                // Continue with next face
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Exception processing face " + String::num(face_index) + " - " + String(e.what()));
                // Continue with next face
            }

            face_explorer.Next();
            face_index++;
        }

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting per-face data - " + String(e.GetMessageString()));
        return Array();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception extracting per-face data - " + String(e.what()));
        return Array();
    }
}

bool ocgd_MeshDataExtractor::has_triangulation(const Ref<ocgd_TopoDS_Shape>& shape) const {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check triangulation - shape reference is null");
            return false;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check triangulation - shape is null");
            return false;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check triangulation - OpenCASCADE shape is null");
            return false;
        }

        // Check all faces for triangulation
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (!triangulation.IsNull()) {
                return true;
            }

            face_explorer.Next();
        }

        return false;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception checking triangulation - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception checking triangulation - " + String(e.what()));
        return false;
    }
}

bool ocgd_MeshDataExtractor::face_has_triangulation(const Ref<ocgd_TopoDS_Shape>& face) const {
    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check face triangulation - face reference is null");
            return false;
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check face triangulation - face is null");
            return false;
        }

        const TopoDS_Shape& occt_shape = face->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check face triangulation - OpenCASCADE shape is null");
            return false;
        }

        if (occt_shape.ShapeType() != TopAbs_FACE) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot check face triangulation - shape is not a face");
            return false;
        }

        const TopoDS_Face& occt_face = TopoDS::Face(occt_shape);

        TopLoc_Location location;
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(occt_face, location);

        return !triangulation.IsNull();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception checking face triangulation - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception checking face triangulation - " + String(e.what()));
        return false;
    }
}

Dictionary ocgd_MeshDataExtractor::get_triangulation_stats(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary stats;

    if (shape.is_null() || shape->is_null()) {
        stats["face_count"] = 0;
        stats["triangle_count"] = 0;
        stats["vertex_count"] = 0;
        return stats;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    int face_count = 0;
    int total_triangles = 0;
    int total_vertices = 0;

    TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
    while (face_explorer.More()) {
        const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

        TopLoc_Location location;
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

        if (!triangulation.IsNull()) {
            face_count++;
            total_triangles += triangulation->NbTriangles();
            total_vertices += triangulation->NbNodes();
        }

        face_explorer.Next();
    }

    stats["face_count"] = face_count;
    stats["triangle_count"] = total_triangles;
    stats["vertex_count"] = total_vertices;

    return stats;
}

Dictionary ocgd_MeshDataExtractor::get_triangulation_bounds(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary bounds;

    if (shape.is_null() || shape->is_null()) {
        bounds["min"] = Vector3(0, 0, 0);
        bounds["max"] = Vector3(0, 0, 0);
        return bounds;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    Bnd_Box bounding_box;
    BRepBndLib::Add(occt_shape, bounding_box);

    if (bounding_box.IsVoid()) {
        bounds["min"] = Vector3(0, 0, 0);
        bounds["max"] = Vector3(0, 0, 0);
        return bounds;
    }

    Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
    bounding_box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

    bounds["min"] = Vector3(
        static_cast<float>(xmin),
        static_cast<float>(ymin),
        static_cast<float>(zmin)
    );
    bounds["max"] = Vector3(
        static_cast<float>(xmax),
        static_cast<float>(ymax),
        static_cast<float>(zmax)
    );

    return bounds;
}

double ocgd_MeshDataExtractor::get_triangulated_area(const Ref<ocgd_TopoDS_Shape>& shape) const {
    if (shape.is_null() || shape->is_null()) {
        return 0.0;
    }

    const TopoDS_Shape& occt_shape = shape->get_occt_shape();

    double total_area = 0.0;

    TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
    while (face_explorer.More()) {
        const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

        TopLoc_Location location;
        Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

        if (!triangulation.IsNull()) {
            total_area += compute_triangulated_area(triangulation, location);
        }

        face_explorer.Next();
    }

    return total_area;
}

Handle(Poly_Triangulation) ocgd_MeshDataExtractor::get_face_triangulation(const TopoDS_Face& face, int purpose) const {
    TopLoc_Location location;
    return BRep_Tool::Triangulation(face, location, static_cast<Poly_MeshPurpose>(purpose));
}

bool ocgd_MeshDataExtractor::validate_triangulation(const Handle(Poly_Triangulation)& triangulation) const {
    if (triangulation.IsNull()) {
        return false;
    }

    try {
        const Standard_Integer nb_nodes = triangulation->NbNodes();
        const Standard_Integer nb_triangles = triangulation->NbTriangles();

        // Basic sanity checks - must have some data
        if (nb_nodes <= 0 || nb_triangles <= 0) {
            return false;
        }

        // Check if we can access basic data using indexed accessors
        try {
            // Test access to first and last nodes
            if (nb_nodes >= 1) {
                triangulation->Node(1);
                triangulation->Node(nb_nodes);
            }

            // Test access to first and last triangles
            if (nb_triangles >= 1) {
                triangulation->Triangle(1);
                triangulation->Triangle(nb_triangles);
            }

        } catch (const Standard_Failure&) {
            // If we can't access basic elements, mark as invalid but don't fail processing
            UtilityFunctions::printerr("MeshDataExtractor: Cannot access triangulation elements via indexed accessors");
            return false;
        } catch (const std::exception&) {
            return false;
        }

        return true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception validating triangulation - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception validating triangulation - " + String(e.what()));
        return false;
    }
}

Dictionary ocgd_MeshDataExtractor::get_detailed_triangulation_info(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary result;

    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot get detailed info - shape reference is null");
            result["error"] = "Shape reference is null";
            return result;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot get detailed info - shape is null");
            result["error"] = "Shape is null";
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot get detailed info - OpenCASCADE shape is null");
            result["error"] = "OpenCASCADE shape is null";
            return result;
        }

        Array face_info_array;
        int face_index = 0;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            Dictionary face_info;
            face_info["face_index"] = face_index;

            try {
                const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
                TopLoc_Location location;
                Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

                if (triangulation.IsNull()) {
                    face_info["has_triangulation"] = false;
                    face_info["error"] = "No triangulation found";
                } else {
                    face_info["has_triangulation"] = true;
                    face_info["is_valid"] = validate_triangulation(triangulation);

                    const Standard_Integer nb_nodes = triangulation->NbNodes();
                    const Standard_Integer nb_triangles = triangulation->NbTriangles();

                    face_info["node_count"] = nb_nodes;
                    face_info["triangle_count"] = nb_triangles;
                    face_info["has_normals"] = triangulation->HasNormals();
                    face_info["has_uv_nodes"] = triangulation->HasUVNodes();

                    // Array bounds info
                    try {
                        // Test indexed access to triangulation data
                        face_info["can_access_nodes"] = true;
                        face_info["can_access_triangles"] = true;

                        try {
                            if (nb_nodes > 0) {
                                triangulation->Node(1);
                                triangulation->Node(nb_nodes);
                            }
                        } catch (...) {
                            face_info["can_access_nodes"] = false;
                        }

                        try {
                            if (nb_triangles > 0) {
                                triangulation->Triangle(1);
                                triangulation->Triangle(nb_triangles);
                            }
                        } catch (...) {
                            face_info["can_access_triangles"] = false;
                        }

                        // Check sample of triangle indices using indexed accessors
                        int invalid_triangles = 0;
                        int checked_triangles = 0;
                        Standard_Integer max_check = std::min(nb_triangles, Standard_Integer(5));

                        for (Standard_Integer i = 1; i <= max_check; i++) {
                            try {
                                const Poly_Triangle& triangle = triangulation->Triangle(i);
                                Standard_Integer n1, n2, n3;
                                triangle.Get(n1, n2, n3);

                                checked_triangles++;
                                if (n1 < 1 || n1 > nb_nodes || n2 < 1 || n2 > nb_nodes || n3 < 1 || n3 > nb_nodes) {
                                    invalid_triangles++;
                                }
                            } catch (...) {
                                // Continue checking other triangles
                            }
                        }

                        face_info["invalid_triangle_sample"] = invalid_triangles;
                        face_info["checked_triangles"] = checked_triangles;

                    } catch (const Standard_Failure& e) {
                        face_info["array_access_error"] = String(e.GetMessageString());
                    } catch (const std::exception& e) {
                        face_info["array_access_error"] = String(e.what());
                    }
                }

            } catch (const Standard_Failure& e) {
                face_info["error"] = String("Exception: ") + String(e.GetMessageString());
            } catch (const std::exception& e) {
                face_info["error"] = String("Exception: ") + String(e.what());
            }

            face_info_array.append(face_info);
            face_explorer.Next();
            face_index++;
        }

        result["faces"] = face_info_array;
        result["total_faces"] = face_index;

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception getting detailed triangulation info - " + String(e.GetMessageString()));
        result["error"] = String("Analysis failed: ") + String(e.GetMessageString());
        return result;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception getting detailed triangulation info - " + String(e.what()));
        result["error"] = String("Analysis failed: ") + String(e.what());
        return result;
    }
}

bool ocgd_MeshDataExtractor::ensure_triangulation(const Ref<ocgd_TopoDS_Shape>& shape, double linear_deflection, double angular_deflection, bool compute_normals) const {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot ensure triangulation - shape reference is null");
            return false;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot ensure triangulation - shape is null");
            return false;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot ensure triangulation - OpenCASCADE shape is null");
            return false;
        }

        // Check if triangulation already exists
        bool needs_triangulation = false;
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (triangulation.IsNull()) {
                needs_triangulation = true;
                break;
            }

            face_explorer.Next();
        }

        if (needs_triangulation) {
            UtilityFunctions::printerr("MeshDataExtractor: Shape needs triangulation, applying mesh...");
            BRepMesh_IncrementalMesh mesh(occt_shape, linear_deflection, Standard_False, angular_deflection);
            mesh.Perform();

            if (!mesh.IsDone()) {
                UtilityFunctions::printerr("MeshDataExtractor: Failed to triangulate shape");
                return false;
            }

            UtilityFunctions::printerr("MeshDataExtractor: Triangulation completed successfully");
        }
        return true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception ensuring triangulation - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception ensuring triangulation - " + String(e.what()));
        return false;
    }
}

Dictionary ocgd_MeshDataExtractor::debug_validate_triangulation(const Ref<ocgd_TopoDS_Shape>& shape) const {
    Dictionary result;

    try {
        if (shape.is_null()) {
            result["error"] = "Shape reference is null";
            return result;
        }

        if (shape->is_null()) {
            result["error"] = "Shape is null";
            return result;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();
        if (occt_shape.IsNull()) {
            result["error"] = "OpenCASCADE shape is null";
            return result;
        }

        Array face_validations;
        int face_index = 0;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            Dictionary face_validation;
            face_validation["face_index"] = face_index;

            try {
                const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
                TopLoc_Location location;
                Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

                if (triangulation.IsNull()) {
                    face_validation["has_triangulation"] = false;
                    face_validation["validation_result"] = "No triangulation";
                } else {
                    face_validation["has_triangulation"] = true;

                    const Standard_Integer nb_nodes = triangulation->NbNodes();
                    const Standard_Integer nb_triangles = triangulation->NbTriangles();

                    face_validation["node_count"] = nb_nodes;
                    face_validation["triangle_count"] = nb_triangles;

                    Array validation_issues;

                    if (nb_nodes <= 0) {
                        validation_issues.append("No nodes found");
                    }
                    if (nb_triangles <= 0) {
                        validation_issues.append("No triangles found");
                    }

                    // Test indexed access
                    bool can_access_nodes = true;
                    bool can_access_triangles = true;

                    try {
                        if (nb_nodes > 0) {
                            triangulation->Node(1);
                            triangulation->Node(nb_nodes);
                        }
                    } catch (...) {
                        can_access_nodes = false;
                        validation_issues.append("Cannot access nodes via indexed accessors");
                    }

                    try {
                        if (nb_triangles > 0) {
                            triangulation->Triangle(1);
                            triangulation->Triangle(nb_triangles);
                        }
                    } catch (...) {
                        can_access_triangles = false;
                        validation_issues.append("Cannot access triangles via indexed accessors");
                    }

                    face_validation["can_access_nodes"] = can_access_nodes;
                    face_validation["can_access_triangles"] = can_access_triangles;

                    // Check sample of triangle indices using indexed accessors
                    if (can_access_triangles) {
                        int bad_triangles = 0;
                        int total_checked = 0;
                        Standard_Integer max_check = std::min(nb_triangles, Standard_Integer(10));

                        for (Standard_Integer i = 1; i <= max_check; i++) {
                            try {
                                const Poly_Triangle& triangle = triangulation->Triangle(i);
                                Standard_Integer n1, n2, n3;
                                triangle.Get(n1, n2, n3);

                                total_checked++;
                                if (n1 < 1 || n1 > nb_nodes || n2 < 1 || n2 > nb_nodes || n3 < 1 || n3 > nb_nodes) {
                                    bad_triangles++;
                                    if (bad_triangles == 1) { // Report first bad triangle
                                        validation_issues.append("Bad triangle indices found: [" + String::num(n1) + ", " + String::num(n2) + ", " + String::num(n3) + "] max_nodes=" + String::num(nb_nodes));
                                    }
                                }
                            } catch (...) {
                                // Continue with next triangle
                            }
                        }

                        face_validation["bad_triangles_sample"] = bad_triangles;
                        face_validation["triangles_checked"] = total_checked;
                    }

                    face_validation["validation_issues"] = validation_issues;
                    face_validation["is_valid"] = validation_issues.size() == 0;
                    face_validation["lenient_valid"] = validate_triangulation(triangulation);
                }

            } catch (const Standard_Failure& e) {
                face_validation["error"] = String("Exception: ") + String(e.GetMessageString());
            } catch (const std::exception& e) {
                face_validation["error"] = String("Exception: ") + String(e.what());
            }

            face_validations.append(face_validation);
            face_explorer.Next();
            face_index++;
        }

        result["face_validations"] = face_validations;
        result["total_faces"] = face_index;

        return result;

    } catch (const Standard_Failure& e) {
        result["error"] = String("Analysis failed: ") + String(e.GetMessageString());
        return result;
    } catch (const std::exception& e) {
        result["error"] = String("Analysis failed: ") + String(e.what());
        return result;
    }
}

bool ocgd_MeshDataExtractor::compute_normals(const Ref<ocgd_TopoDS_Shape>& shape) const {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot compute normals - shape reference is null");
            return false;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot compute normals - shape is null");
            return false;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot compute normals - OpenCASCADE shape is null");
            return false;
        }

        UtilityFunctions::printerr("MeshDataExtractor: Computing normals for shape using enhanced algorithms");
        int faces_processed = 0;
        int faces_with_surface_normals = 0;
        int faces_with_triangle_normals = 0;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (!triangulation.IsNull()) {
                const Standard_Integer nb_nodes = triangulation->NbNodes();
                const Standard_Integer nb_triangles = triangulation->NbTriangles();

                if (nb_nodes > 0 && nb_triangles > 0) {
                    // Use enhanced normal computation
                    if (ocgd_EnhancedNormals::compute_and_store_normals(face, triangulation)) {
                        faces_processed++;

                        // Get diagnostic info to track computation method
                        Dictionary info = ocgd_EnhancedNormals::get_normal_computation_info(face, triangulation);
                        String method = info.get("computation_method", "unknown");
                        if (method == "surface_based") {
                            faces_with_surface_normals++;
                        } else if (method == "triangle_based") {
                            faces_with_triangle_normals++;
                        }
                    } else {
                        UtilityFunctions::printerr("MeshDataExtractor: Failed to compute normals for face, skipping");
                    }
                }
            }

            face_explorer.Next();
        }

        UtilityFunctions::printerr("MeshDataExtractor: Enhanced normal computation completed for " +
                                 String::num(faces_processed) + " faces (" +
                                 String::num(faces_with_surface_normals) + " surface-based, " +
                                 String::num(faces_with_triangle_normals) + " triangle-based)");
        return faces_processed > 0;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception computing normals - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception computing normals - " + String(e.what()));
        return false;
    }
}

Dictionary ocgd_MeshDataExtractor::convert_triangulation_to_dict(const Handle(Poly_Triangulation)& triangulation,
                                                               const TopLoc_Location& location,
                                                               const TopoDS_Face* face) const {
    Dictionary result;

    try {
        if (triangulation.IsNull()) {
            UtilityFunctions::printerr("MeshDataExtractor: Cannot convert triangulation - triangulation is null");
            return result;
        }

        // Light validation - only skip if severely corrupted
        if (!validate_triangulation(triangulation)) {
            UtilityFunctions::printerr("MeshDataExtractor: Triangulation severely corrupted - attempting safe conversion anyway");
            // Don't return early - try to process what we can
        }

        const Standard_Integer nb_nodes = triangulation->NbNodes();
        const Standard_Integer nb_triangles = triangulation->NbTriangles();

        if (nb_nodes <= 0) {
            UtilityFunctions::printerr("MeshDataExtractor: Invalid triangulation - no nodes found, but continuing");
        }

        if (nb_triangles <= 0) {
            UtilityFunctions::printerr("MeshDataExtractor: Invalid triangulation - no triangles found, but continuing");
        }

        // Extract vertices using indexed accessors
        PackedVector3Array vertices;

        for (Standard_Integer i = 1; i <= nb_nodes; i++) {
            try {
                gp_Pnt point = triangulation->Node(i);

                // Apply location transformation if present
                if (!location.IsIdentity()) {
                    point.Transform(location.Transformation());
                }

                vertices.append(Vector3(
                    static_cast<float>(point.X()),
                    static_cast<float>(point.Y()),
                    static_cast<float>(point.Z())
                ));
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Failed to access node " + String::num(i) + " - " + String(e.GetMessageString()) + ", continuing");
                // Add a default vertex to maintain indexing
                vertices.append(Vector3(0, 0, 0));
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Failed to access node " + String::num(i) + " - " + String(e.what()) + ", continuing");
                vertices.append(Vector3(0, 0, 0));
            }
        }

        // Extract triangles using indexed accessors
        PackedInt32Array triangles;

        // Check face orientation for triangle winding order
        Standard_Boolean is_reversed = (face != nullptr && face->Orientation() == TopAbs_REVERSED);

        for (Standard_Integer i = 1; i <= nb_triangles; i++) {
            try {
                const Poly_Triangle& triangle = triangulation->Triangle(i);
                Standard_Integer n1, n2, n3;
                triangle.Get(n1, n2, n3);

                // Validate triangle node indices but continue even if some are bad
                if (n1 < 1 || n1 > nb_nodes || n2 < 1 || n2 > nb_nodes || n3 < 1 || n3 > nb_nodes) {
                    UtilityFunctions::printerr("MeshDataExtractor: Invalid triangle node indices - Triangle " + String::num(i) +
                                             ": n1=" + String::num(n1) + ", n2=" + String::num(n2) + ", n3=" + String::num(n3) +
                                             ", max_nodes=" + String::num(nb_nodes) + ", skipping triangle");
                    continue;
                }

                // Convert to 0-based indexing and handle face orientation
                if (is_reversed) {
                    triangles.append(n1 - 1);
                    triangles.append(n2 - 1);
                    triangles.append(n3 - 1);
                } else {
                    triangles.append(n1 - 1);
                    triangles.append(n3 - 1);
                    triangles.append(n2 - 1);
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Failed to access triangle " + String::num(i) + " - " + String(e.GetMessageString()) + ", continuing");
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("MeshDataExtractor: Failed to access triangle " + String::num(i) + " - " + String(e.what()) + ", continuing");
            }
        }

        result["vertices"] = vertices;
        result["triangles"] = triangles;

        // Extract or compute normals if requested
        if (_include_normals) {
            PackedVector3Array normals;

            if (triangulation->HasNormals()) {
                // Extract existing normals
                for (Standard_Integer i = 1; i <= nb_nodes; i++) {
                    try {
                        // Get normal using triangulation API
                        gp_Dir normal_dir = triangulation->Normal(i);

                        Vector3 normal(
                            static_cast<float>(normal_dir.X()),
                            static_cast<float>(normal_dir.Y()),
                            static_cast<float>(normal_dir.Z())
                        );

                        // Apply location transformation to normals if present
                        if (!location.IsIdentity()) {
                            gp_Vec normal_vec(normal.x, normal.y, normal.z);
                            normal_vec.Transform(location.Transformation());
                            normal = Vector3(
                                static_cast<float>(normal_vec.X()),
                                static_cast<float>(normal_vec.Y()),
                                static_cast<float>(normal_vec.Z())
                            );
                        }

                        normals.append(normal);
                    } catch (const Standard_Failure& e) {
                        UtilityFunctions::printerr("MeshDataExtractor: Failed to access normal " + String::num(i) + " - " + String(e.GetMessageString()) + ", using default");
                        normals.append(Vector3(0, 0, 1)); // Default normal
                    } catch (const std::exception& e) {
                        UtilityFunctions::printerr("MeshDataExtractor: Failed to access normal " + String::num(i) + " - " + String(e.what()) + ", using default");
                        normals.append(Vector3(0, 0, 1));
                    }
                }
            } else {
                // Compute normals from triangle geometry
                UtilityFunctions::printerr("MeshDataExtractor: Computing normals from triangle geometry");

                // Initialize normals array with zeros
                normals.resize(vertices.size());
                for (int i = 0; i < normals.size(); i++) {
                    normals[i] = Vector3(0, 0, 0);
                }

                // Compute normals by accumulating triangle normals at each vertex
                for (int tri_idx = 0; tri_idx < triangles.size(); tri_idx += 3) {
                    if (tri_idx + 2 < triangles.size()) {
                        int idx1 = triangles[tri_idx];
                        int idx2 = triangles[tri_idx + 1];
                        int idx3 = triangles[tri_idx + 2];

                        if (idx1 < vertices.size() && idx2 < vertices.size() && idx3 < vertices.size()) {
                            Vector3 v1 = vertices[idx1];
                            Vector3 v2 = vertices[idx2];
                            Vector3 v3 = vertices[idx3];

                            // Calculate triangle normal using cross product
                            Vector3 edge1 = v2 - v1;
                            Vector3 edge2 = v3 - v1;
                            Vector3 triangle_normal = edge1.cross(edge2);

                            if (triangle_normal.length() > 1e-6) {
                                triangle_normal = triangle_normal.normalized();

                                // Accumulate normal at each vertex
                                normals[idx1] += triangle_normal;
                                normals[idx2] += triangle_normal;
                                normals[idx3] += triangle_normal;
                            }
                        }
                    }
                }

                // Normalize accumulated normals
                for (int i = 0; i < normals.size(); i++) {
                    if (normals[i].length() > 1e-6) {
                        normals[i] = normals[i].normalized();
                    } else {
                        normals[i] = Vector3(0, 0, 1); // Default normal for degenerate cases
                    }
                }
            }

            if (normals.size() > 0) {
                result["normals"] = normals;
            }
        }

        // Extract UV coordinates if available and requested
        if (_include_uvs && triangulation->HasUVNodes()) {
            PackedVector2Array uvs;

            for (Standard_Integer i = 1; i <= nb_nodes; i++) {
                try {
                    // Use indexed accessor for UV coordinates
                    const gp_Pnt2d& uv = triangulation->UVNode(i);
                    uvs.append(Vector2(
                        static_cast<float>(uv.X()),
                        static_cast<float>(uv.Y())
                    ));
                } catch (const Standard_Failure& e) {
                    UtilityFunctions::printerr("MeshDataExtractor: Failed to access UV " + String::num(i) + " - " + String(e.GetMessageString()) + ", using default");
                    uvs.append(Vector2(0, 0)); // Default UV
                } catch (const std::exception& e) {
                    UtilityFunctions::printerr("MeshDataExtractor: Failed to access UV " + String::num(i) + " - " + String(e.what()) + ", using default");
                    uvs.append(Vector2(0, 0));
                }
            }

            if (uvs.size() > 0) {
                result["uvs"] = uvs;
            }
        }

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception converting triangulation to dict - " + String(e.GetMessageString()));
        return Dictionary();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("MeshDataExtractor: Exception converting triangulation to dict - " + String(e.what()));
        return Dictionary();
    }
}

void ocgd_MeshDataExtractor::apply_transformation(PackedVector3Array& vertices, const gp_Trsf& transformation) const {
    for (int i = 0; i < vertices.size(); i++) {
        Vector3 vertex = vertices[i];
        gp_Pnt point(vertex.x, vertex.y, vertex.z);
        point.Transform(transformation);
        vertices[i] = Vector3(
            static_cast<float>(point.X()),
            static_cast<float>(point.Y()),
            static_cast<float>(point.Z())
        );
    }
}

void ocgd_MeshDataExtractor::merge_duplicate_vertices(PackedVector3Array& vertices, PackedInt32Array& triangles,
                                                     PackedVector3Array& normals, PackedVector2Array& uvs) const {
    if (vertices.size() == 0) return;

    // Simple vertex merging implementation
    std::vector<int> index_map(vertices.size());
    std::vector<Vector3> unique_vertices;
    std::vector<Vector3> unique_normals;
    std::vector<Vector2> unique_uvs;

    const double tolerance_sq = _vertex_merge_tolerance * _vertex_merge_tolerance;

    for (int i = 0; i < vertices.size(); i++) {
        const Vector3& vertex = vertices[i];
        bool found = false;

        // Look for existing vertex within tolerance
        for (int j = 0; j < unique_vertices.size(); j++) {
            const Vector3& existing = unique_vertices[j];
            double dist_sq = vertex.distance_squared_to(existing);

            if (dist_sq < tolerance_sq) {
                index_map[i] = j;
                found = true;
                break;
            }
        }

        if (!found) {
            // Add new unique vertex
            index_map[i] = unique_vertices.size();
            unique_vertices.push_back(vertex);

            if (i < normals.size()) {
                unique_normals.push_back(normals[i]);
            }

            if (i < uvs.size()) {
                unique_uvs.push_back(uvs[i]);
            }
        }
    }

    // Rebuild arrays with unique vertices
    vertices.clear();
    normals.clear();
    uvs.clear();

    for (const Vector3& vertex : unique_vertices) {
        vertices.append(vertex);
    }

    for (const Vector3& normal : unique_normals) {
        normals.append(normal);
    }

    for (const Vector2& uv : unique_uvs) {
        uvs.append(uv);
    }

    // Update triangle indices
    for (int i = 0; i < triangles.size(); i++) {
        triangles[i] = index_map[triangles[i]];
    }
}

double ocgd_MeshDataExtractor::compute_triangulated_area(const Handle(Poly_Triangulation)& triangulation,
                                                       const TopLoc_Location& location) const {
    if (triangulation.IsNull()) {
        return 0.0;
    }

    double total_area = 0.0;

    for (Standard_Integer i = 1; i <= triangulation->NbTriangles(); i++) {
        try {
            const Poly_Triangle& triangle = triangulation->Triangle(i);
            Standard_Integer n1, n2, n3;
            triangle.Get(n1, n2, n3);

            gp_Pnt p1 = triangulation->Node(n1);
            gp_Pnt p2 = triangulation->Node(n2);
            gp_Pnt p3 = triangulation->Node(n3);

            // Apply location transformation if present
            if (!location.IsIdentity()) {
                p1.Transform(location.Transformation());
                p2.Transform(location.Transformation());
                p3.Transform(location.Transformation());
            }

            // Calculate triangle area using cross product
            gp_Vec v1(p1, p2);
            gp_Vec v2(p1, p3);
            gp_Vec cross = v1.Crossed(v2);
            double area = 0.5 * cross.Magnitude();

            total_area += area;
        } catch (const Standard_Failure& e) {
            UtilityFunctions::printerr("MeshDataExtractor: Failed to access triangle " + String::num(i) + " for area calculation - " + String(e.GetMessageString()) + ", continuing");
            // Continue with next triangle
        } catch (const std::exception& e) {
            UtilityFunctions::printerr("MeshDataExtractor: Failed to access triangle " + String::num(i) + " for area calculation - " + String(e.what()) + ", continuing");
            // Continue with next triangle
        }
    }

    return total_area;
}

void ocgd_MeshDataExtractor::_bind_methods() {
    // Property getters and setters
    ClassDB::bind_method(D_METHOD("set_include_normals", "include"), &ocgd_MeshDataExtractor::set_include_normals);
    ClassDB::bind_method(D_METHOD("get_include_normals"), &ocgd_MeshDataExtractor::get_include_normals);
    ClassDB::add_property("ocgd_MeshDataExtractor", PropertyInfo(Variant::BOOL, "include_normals"), "set_include_normals", "get_include_normals");

    ClassDB::bind_method(D_METHOD("set_include_uvs", "include"), &ocgd_MeshDataExtractor::set_include_uvs);
    ClassDB::bind_method(D_METHOD("get_include_uvs"), &ocgd_MeshDataExtractor::get_include_uvs);
    ClassDB::add_property("ocgd_MeshDataExtractor", PropertyInfo(Variant::BOOL, "include_uvs"), "set_include_uvs", "get_include_uvs");

    ClassDB::bind_method(D_METHOD("set_merge_vertices", "merge"), &ocgd_MeshDataExtractor::set_merge_vertices);
    ClassDB::bind_method(D_METHOD("get_merge_vertices"), &ocgd_MeshDataExtractor::get_merge_vertices);
    ClassDB::add_property("ocgd_MeshDataExtractor", PropertyInfo(Variant::BOOL, "merge_vertices"), "set_merge_vertices", "get_merge_vertices");

    ClassDB::bind_method(D_METHOD("set_vertex_merge_tolerance", "tolerance"), &ocgd_MeshDataExtractor::set_vertex_merge_tolerance);
    ClassDB::bind_method(D_METHOD("get_vertex_merge_tolerance"), &ocgd_MeshDataExtractor::get_vertex_merge_tolerance);
    ClassDB::add_property("ocgd_MeshDataExtractor", PropertyInfo(Variant::FLOAT, "vertex_merge_tolerance"), "set_vertex_merge_tolerance", "get_vertex_merge_tolerance");

    // Main extraction methods
    ClassDB::bind_method(D_METHOD("extract_mesh_data", "shape", "linear_deflection", "angular_deflection", "compute_normals"), &ocgd_MeshDataExtractor::extract_mesh_data, DEFVAL(0.1), DEFVAL(0.1), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("extract_face_data", "face", "linear_deflection", "angular_deflection", "compute_normals"), &ocgd_MeshDataExtractor::extract_face_data, DEFVAL(0.1), DEFVAL(0.1), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("extract_per_face_data", "shape", "linear_deflection", "angular_deflection", "compute_normals"), &ocgd_MeshDataExtractor::extract_per_face_data, DEFVAL(0.1), DEFVAL(0.1), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("extract_mesh_data_with_purpose", "shape", "linear_deflection", "angular_deflection", "purpose", "compute_normals"), &ocgd_MeshDataExtractor::extract_mesh_data_with_purpose, DEFVAL(0.1), DEFVAL(0.1), DEFVAL(0), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("extract_vertices", "shape"), &ocgd_MeshDataExtractor::extract_vertices);
    ClassDB::bind_method(D_METHOD("extract_triangles", "shape"), &ocgd_MeshDataExtractor::extract_triangles);
    ClassDB::bind_method(D_METHOD("extract_normals", "shape"), &ocgd_MeshDataExtractor::extract_normals);
    ClassDB::bind_method(D_METHOD("extract_uvs", "shape"), &ocgd_MeshDataExtractor::extract_uvs);
    ClassDB::bind_method(D_METHOD("extract_per_face_data", "shape"), &ocgd_MeshDataExtractor::extract_per_face_data);
    ClassDB::bind_method(D_METHOD("extract_mesh_data_with_purpose", "shape", "purpose"), &ocgd_MeshDataExtractor::extract_mesh_data_with_purpose);

    // Query methods
    ClassDB::bind_method(D_METHOD("has_triangulation", "shape"), &ocgd_MeshDataExtractor::has_triangulation);
    ClassDB::bind_method(D_METHOD("face_has_triangulation", "face"), &ocgd_MeshDataExtractor::face_has_triangulation);
    ClassDB::bind_method(D_METHOD("get_triangulation_stats", "shape"), &ocgd_MeshDataExtractor::get_triangulation_stats);
    ClassDB::bind_method(D_METHOD("get_triangulation_bounds", "shape"), &ocgd_MeshDataExtractor::get_triangulation_bounds);
    ClassDB::bind_method(D_METHOD("get_triangulated_area", "shape"), &ocgd_MeshDataExtractor::get_triangulated_area);
    ClassDB::bind_method(D_METHOD("get_detailed_triangulation_info", "shape"), &ocgd_MeshDataExtractor::get_detailed_triangulation_info);
    ClassDB::bind_method(D_METHOD("debug_validate_triangulation", "shape"), &ocgd_MeshDataExtractor::debug_validate_triangulation);
    ClassDB::bind_method(D_METHOD("ensure_triangulation", "shape", "linear_deflection", "angular_deflection", "compute_normals"), &ocgd_MeshDataExtractor::ensure_triangulation, DEFVAL(0.1), DEFVAL(0.1), DEFVAL(false));
    ClassDB::bind_method(D_METHOD("compute_normals", "shape"), &ocgd_MeshDataExtractor::compute_normals);

    // Mesh purpose enum constants
    BIND_ENUM_CONSTANT(MESH_PURPOSE_NONE);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_USER);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_PRESENTATION);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_CALCULATION);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_ACTIVE);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_LOADED);
    BIND_ENUM_CONSTANT(MESH_PURPOSE_ANY_FALLBACK);
}
