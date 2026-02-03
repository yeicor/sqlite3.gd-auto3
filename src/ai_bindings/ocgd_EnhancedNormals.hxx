/**
 * ocgd_EnhancedNormals.hxx
 *
 * Godot GDExtension wrapper header for OpenCASCADE enhanced normal computation functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#ifndef OCGD_ENHANCED_NORMALS_HXX
#define OCGD_ENHANCED_NORMALS_HXX

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/Poly_Triangulation.hxx>

using namespace godot;

/**
 * @brief Enhanced normal computation utility class for OpenCASCADE surfaces.
 *
 * This class provides sophisticated algorithms for calculating high-quality surface normals
 * from triangulated OpenCASCADE shapes. It uses surface-based computation when possible,
 * falling back to triangle-based methods when necessary.
 */
class ocgd_EnhancedNormals {
public:
    /**
     * @brief Compute and store normals directly in the triangulation.
     * @param face The face to compute normals for
     * @param triangulation The triangulation to store normals in
     * @return true if normals were successfully computed and stored
     */
    static bool compute_and_store_normals(const TopoDS_Face& face, Handle(Poly_Triangulation)& triangulation);

    /**
     * @brief Extract normals from a triangulation as a Vector3 array.
     * @param face The face the triangulation belongs to
     * @param triangulation The triangulation containing normals
     * @param location The location transformation for the face
     * @return PackedVector3Array containing the normals
     */
    static PackedVector3Array extract_normals_as_vector3_array(const TopoDS_Face& face, 
                                                               const Handle(Poly_Triangulation)& triangulation,
                                                               const TopLoc_Location& location);

    /**
     * @brief Get diagnostic information about normal computation for a face.
     * @param face The face to analyze
     * @param triangulation The triangulation to analyze
     * @return Dictionary containing computation method and quality metrics
     */
    static Dictionary get_normal_computation_info(const TopoDS_Face& face, 
                                                  const Handle(Poly_Triangulation)& triangulation);

    /**
     * @brief Compute surface normal at UV coordinates using OpenCASCADE surface properties.
     * @param face The face to compute normal for
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return Vector3 representing the surface normal
     */
    static Vector3 compute_surface_normal_at_uv(const TopoDS_Face& face, double u, double v);

    /**
     * @brief Set tolerance for normal computations.
     * @param tolerance The tolerance value (must be positive)
     */
    static void set_tolerance(double tolerance);

    /**
     * @brief Get current tolerance value.
     * @return Current tolerance
     */
    static double get_tolerance();

    /**
     * @brief Set whether to flip normals for reversed faces.
     * @param flip_for_reversed If true, normals are flipped for TopAbs_REVERSED faces
     */
    static void set_flip_normals_for_reversed(bool flip_for_reversed);

    /**
     * @brief Get whether normals are flipped for reversed faces.
     * @return Current setting for normal flipping
     */
    static bool get_flip_normals_for_reversed();

private:
    /**
     * @brief Compute normals using surface-based method.
     * @param face The face to compute normals for
     * @param triangulation The triangulation to store normals in
     * @return true if successful
     */
    static bool compute_surface_based_normals(const TopoDS_Face& face, Handle(Poly_Triangulation)& triangulation);

    /**
     * @brief Compute normals using triangle-based method as fallback.
     * @param triangulation The triangulation to store normals in
     * @return true if successful
     */
    static bool compute_triangle_based_normals(Handle(Poly_Triangulation)& triangulation);

    /**
     * @brief Get UV coordinates for a node in the triangulation.
     * @param face The face containing the triangulation
     * @param triangulation The triangulation
     * @param node_index The index of the node (1-based)
     * @param u Output U coordinate
     * @param v Output V coordinate
     * @return true if UV coordinates were successfully retrieved
     */
    static bool get_node_uv(const TopoDS_Face& face, 
                           const Handle(Poly_Triangulation)& triangulation,
                           int node_index, 
                           double& u, 
                           double& v);

    /**
     * @brief Check if surface normals can be computed for this face.
     * @param face The face to check
     * @return true if surface-based computation is possible
     */
    static bool can_compute_surface_normals(const TopoDS_Face& face);

    /**
     * @brief Convert gp_Vec to Vector3.
     * @param vec OpenCASCADE vector
     * @return Godot Vector3
     */
    static Vector3 gp_vec_to_vector3(const gp_Vec& vec);

    /**
     * @brief Convert gp_Dir to Vector3.
     * @param dir OpenCASCADE direction
     * @return Godot Vector3
     */
    static Vector3 gp_dir_to_vector3(const gp_Dir& dir);

    /**
     * @brief Apply location transformation to a vector.
     * @param vec The vector to transform
     * @param location The location transformation
     * @return Transformed vector
     */
    static Vector3 apply_location_to_vector(const Vector3& vec, const TopLoc_Location& location);

    static double s_tolerance; ///< Tolerance for geometric computations
    static bool s_flip_normals_for_reversed; ///< Whether to flip normals for reversed faces
};

#endif // OCGD_ENHANCED_NORMALS_HXX