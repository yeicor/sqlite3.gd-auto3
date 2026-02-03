/**
 * ocgd_EnhancedNormals.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE enhanced normal computation functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_EnhancedNormals.hxx"

#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/BRepAdaptor_Surface.hxx>
#include <opencascade/BRepLProp_SLProps.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/Poly_Triangle.hxx>
#include <opencascade/Poly_Connect.hxx>
#include <opencascade/TColgp_Array1OfDir.hxx>
#include <opencascade/TColgp_Array1OfPnt.hxx>
#include <opencascade/TColgp_Array1OfPnt2d.hxx>
#include <opencascade/Standard_Failure.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Dir.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/TopAbs.hxx>
#include <vector>

using namespace godot;

// Static member initialization
double ocgd_EnhancedNormals::s_tolerance = 1e-6;
bool ocgd_EnhancedNormals::s_flip_normals_for_reversed = false;

bool ocgd_EnhancedNormals::compute_and_store_normals(const TopoDS_Face& face, Handle(Poly_Triangulation)& triangulation) {
    try {
        if (triangulation.IsNull()) {
            UtilityFunctions::printerr("EnhancedNormals: Cannot compute normals - triangulation is null");
            return false;
        }

        const Standard_Integer nb_nodes = triangulation->NbNodes();
        const Standard_Integer nb_triangles = triangulation->NbTriangles();

        if (nb_nodes <= 0 || nb_triangles <= 0) {
            UtilityFunctions::printerr("EnhancedNormals: Cannot compute normals - empty triangulation");
            return false;
        }

        // Debug: Report face orientation (triangle winding handles orientation, normals use raw OpenCASCADE values)
        String orientation_str = (face.Orientation() == TopAbs_REVERSED) ? "REVERSED" : "FORWARD";
        // UtilityFunctions::printerr("EnhancedNormals: Processing face with orientation: " + orientation_str +
        //                          " (triangle winding handles orientation, normal_flip=" + String(s_flip_normals_for_reversed ? "true" : "false") + ")");

        // Try surface-based computation first (higher quality)
        if (can_compute_surface_normals(face)) {
            if (compute_surface_based_normals(face, triangulation)) {
                return true;
            }
            UtilityFunctions::printerr("EnhancedNormals: Surface-based computation failed, falling back to triangle-based");
        }

        // Fallback to triangle-based computation
        return false;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to compute and store normals - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to compute and store normals - " + String(e.what()));
        return false;
    }
}

PackedVector3Array ocgd_EnhancedNormals::extract_normals_as_vector3_array(const TopoDS_Face& face,
                                                                          const Handle(Poly_Triangulation)& triangulation,
                                                                          const TopLoc_Location& location) {
    PackedVector3Array result;

    try {
        if (triangulation.IsNull()) {
            UtilityFunctions::printerr("EnhancedNormals: Cannot extract normals - triangulation is null");
            return result;
        }

        const Standard_Integer nb_nodes = triangulation->NbNodes();

        if (triangulation->HasNormals()) {
            result.resize(nb_nodes);
            for (Standard_Integer i = 1; i <= nb_nodes; i++) {
                gp_Dir normal_dir = triangulation->Normal(i);
                Vector3 normal = gp_dir_to_vector3(normal_dir);

                // Apply location transformation if present
                if (!location.IsIdentity()) {
                    normal = apply_location_to_vector(normal, location);
                }

                result[i - 1] = normal;
            }
        } else {
            UtilityFunctions::printerr("EnhancedNormals: Triangulation does not have normals - computing on demand");

            // Compute normals on demand using surface method if possible
            if (can_compute_surface_normals(face) && triangulation->HasUVNodes()) {
                for (Standard_Integer i = 1; i <= nb_nodes; i++) {
                    double u, v;
                    Vector3 normal;

                    if (get_node_uv(face, triangulation, i, u, v)) {
                        normal = compute_surface_normal_at_uv(face, u, v);
                    } else {
                        normal = Vector3(0, 0, 1); // Default normal
                    }

                    // Triangle winding order already handles orientation, only flip if explicitly requested
                    if (s_flip_normals_for_reversed && face.Orientation() == TopAbs_REVERSED) {
                        normal = -normal;
                    }

                    // Apply location transformation if present
                    if (!location.IsIdentity()) {
                        normal = apply_location_to_vector(normal, location);
                    }

                    result.append(normal);
                }
            } else {
                // Fallback to triangle-based computation
                const Standard_Integer nb_triangles = triangulation->NbTriangles();

                // Create array to store accumulated normals
                std::vector<gp_Vec> accumulated_normals(nb_nodes + 1, gp_Vec(0, 0, 0));
                std::vector<int> normal_counts(nb_nodes + 1, 0);

                // Compute triangle normals and accumulate at vertices
                for (Standard_Integer tri_idx = 1; tri_idx <= nb_triangles; tri_idx++) {
                    const Poly_Triangle& triangle = triangulation->Triangle(tri_idx);
                    Standard_Integer n1, n2, n3;
                    triangle.Get(n1, n2, n3);

                    const gp_Pnt& p1 = triangulation->Node(n1);
                    const gp_Pnt& p2 = triangulation->Node(n2);
                    const gp_Pnt& p3 = triangulation->Node(n3);

                    // Compute triangle normal (triangle winding order already handles orientation)
                    gp_Vec v1(p1, p2);
                    gp_Vec v2(p1, p3);
                    gp_Vec normal = v1.Crossed(v2);

                    // Triangle winding order already handles orientation, only flip if explicitly requested
                    if (s_flip_normals_for_reversed && face.Orientation() == TopAbs_REVERSED) {
                        normal = -normal;
                    }

                    if (normal.Magnitude() > s_tolerance) {
                        normal.Normalize();

                        // Accumulate normal at each vertex
                        accumulated_normals[n1] += normal;
                        accumulated_normals[n2] += normal;
                        accumulated_normals[n3] += normal;

                        normal_counts[n1]++;
                        normal_counts[n2]++;
                        normal_counts[n3]++;
                    }
                }

                // Convert accumulated normals to Vector3 array
                for (Standard_Integer i = 1; i <= nb_nodes; i++) {
                    Vector3 normal;
                    if (normal_counts[i] > 0) {
                        gp_Vec avg_normal = accumulated_normals[i] / normal_counts[i];
                        if (avg_normal.Magnitude() > s_tolerance) {
                            avg_normal.Normalize();
                            normal = gp_vec_to_vector3(avg_normal);
                        } else {
                            normal = Vector3(0, 0, 1);
                        }
                    } else {
                        normal = Vector3(0, 0, 1);
                    }

                    // Triangle winding order already handles orientation, only flip if explicitly requested
                    if (s_flip_normals_for_reversed && face.Orientation() == TopAbs_REVERSED) {
                        normal = -normal;
                    }

                    // Apply location transformation if present
                    if (!location.IsIdentity()) {
                        normal = apply_location_to_vector(normal, location);
                    }

                    result.append(normal);
                }
            }
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to extract normals - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to extract normals - " + String(e.what()));
    }

    return result;
}

Dictionary ocgd_EnhancedNormals::get_normal_computation_info(const TopoDS_Face& face,
                                                            const Handle(Poly_Triangulation)& triangulation) {
    Dictionary result;

    try {
        if (triangulation.IsNull()) {
            result["valid"] = false;
            result["error"] = "Triangulation is null";
            return result;
        }

        const Standard_Integer nb_nodes = triangulation->NbNodes();
        const Standard_Integer nb_triangles = triangulation->NbTriangles();

        result["valid"] = true;
        result["node_count"] = nb_nodes;
        result["triangle_count"] = nb_triangles;
        result["has_normals"] = triangulation->HasNormals();
        result["has_uv_nodes"] = triangulation->HasUVNodes();
        result["can_compute_surface_normals"] = can_compute_surface_normals(face);

        // Determine computation method
        if (triangulation->HasNormals()) {
            result["computation_method"] = "pre_computed";
        } else if (can_compute_surface_normals(face)) {
            result["computation_method"] = "surface_based";
        } else {
            result["computation_method"] = "triangle_based";
        }

        result["tolerance"] = s_tolerance;

    } catch (const Standard_Failure& e) {
        result["valid"] = false;
        result["error"] = String("OpenCASCADE error: ") + String(e.GetMessageString());
    } catch (const std::exception& e) {
        result["valid"] = false;
        result["error"] = String("Standard error: ") + String(e.what());
    }

    return result;
}

Vector3 ocgd_EnhancedNormals::compute_surface_normal_at_uv(const TopoDS_Face& face, double u, double v) {
    try {
        BRepAdaptor_Surface surface(face);
        BRepLProp_SLProps props(surface, u, v, 1, s_tolerance);

        if (props.IsNormalDefined()) {
            gp_Dir normal = props.Normal();
            Vector3 result = gp_dir_to_vector3(normal);

            // Triangle winding order already handles orientation, only flip if explicitly requested
            if (s_flip_normals_for_reversed && face.Orientation() == TopAbs_REVERSED) {
                result = -result;
            }

            return result;
        } else {
            UtilityFunctions::printerr("EnhancedNormals: Normal not defined at UV (" + String::num(u) + ", " + String::num(v) + ")");
            return Vector3(0, 0, 1);
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to compute surface normal at UV - " + String(e.GetMessageString()));
        return Vector3(0, 0, 1);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to compute surface normal at UV - " + String(e.what()));
        return Vector3(0, 0, 1);
    }
}

void ocgd_EnhancedNormals::set_tolerance(double tolerance) {
    if (tolerance > 0.0) {
        s_tolerance = tolerance;
    } else {
        UtilityFunctions::printerr("EnhancedNormals: Tolerance must be positive, using default");
        s_tolerance = 1e-6;
    }
}

double ocgd_EnhancedNormals::get_tolerance() {
    return s_tolerance;
}

void ocgd_EnhancedNormals::set_flip_normals_for_reversed(bool flip_for_reversed) {
    s_flip_normals_for_reversed = flip_for_reversed;
    UtilityFunctions::printerr("EnhancedNormals: Normal flipping for reversed faces set to " + String(flip_for_reversed ? "true" : "false") +
                             " (triangle winding order already handles orientation)");
}

bool ocgd_EnhancedNormals::get_flip_normals_for_reversed() {
    return s_flip_normals_for_reversed;
}

bool ocgd_EnhancedNormals::compute_surface_based_normals(const TopoDS_Face& face, Handle(Poly_Triangulation)& triangulation) {
    try {
        const Standard_Integer nb_nodes = triangulation->NbNodes();

        // Check if we have UV nodes in the triangulation
        if (!triangulation->HasUVNodes()) {
            UtilityFunctions::printerr("EnhancedNormals: Cannot compute surface normals - no UV nodes available");
            return false;
        }

        // Just mark that we can compute surface normals - actual computation done on-demand
        return true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Surface-based normal computation failed - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Surface-based normal computation failed - " + String(e.what()));
        return false;
    }
}

bool ocgd_EnhancedNormals::get_node_uv(const TopoDS_Face& face,
                                       const Handle(Poly_Triangulation)& triangulation,
                                       int node_index,
                                       double& u,
                                       double& v) {
    try {
        if (triangulation.IsNull() || !triangulation->HasUVNodes()) {
            return false;
        }

        if (node_index < 1 || node_index > triangulation->NbNodes()) {
            return false;
        }

        gp_Pnt2d uv = triangulation->UVNode(node_index);
        u = uv.X();
        v = uv.Y();

        return true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to get node UV - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to get node UV - " + String(e.what()));
        return false;
    }
}

bool ocgd_EnhancedNormals::can_compute_surface_normals(const TopoDS_Face& face) {
    try {
        // Check if we can create a surface adaptor and if it supports normal computation
        BRepAdaptor_Surface surface(face);

        // Try to get UV bounds to verify the surface is well-defined
        Standard_Real u_min, u_max, v_min, v_max;
        BRepTools::UVBounds(face, u_min, u_max, v_min, v_max);

        // Check if bounds are reasonable
        if (u_max <= u_min || v_max <= v_min) {
            return false;
        }

        // Test normal computation at center point
        Standard_Real u_mid = (u_min + u_max) / 2.0;
        Standard_Real v_mid = (v_min + v_max) / 2.0;

        BRepLProp_SLProps props(surface, u_mid, v_mid, 1, s_tolerance);
        return props.IsNormalDefined();

    } catch (const Standard_Failure& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

Vector3 ocgd_EnhancedNormals::gp_vec_to_vector3(const gp_Vec& vec) {
    return Vector3(vec.X(), vec.Y(), vec.Z());
}

Vector3 ocgd_EnhancedNormals::gp_dir_to_vector3(const gp_Dir& dir) {
    return Vector3(dir.X(), dir.Y(), dir.Z());
}

Vector3 ocgd_EnhancedNormals::apply_location_to_vector(const Vector3& vec, const TopLoc_Location& location) {
    try {
        if (location.IsIdentity()) {
            return vec;
        }

        // Convert Vector3 to gp_Vec
        gp_Vec occt_vec(vec.x, vec.y, vec.z);

        // Apply the transformation
        const gp_Trsf& transformation = location.Transformation();
        occt_vec.Transform(transformation);

        // Convert back to Vector3
        return Vector3(occt_vec.X(), occt_vec.Y(), occt_vec.Z());

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to apply location transformation - " + String(e.GetMessageString()));
        return vec; // Return original vector on failure
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("EnhancedNormals: Failed to apply location transformation - " + String(e.what()));
        return vec; // Return original vector on failure
    }
}
