/**
 * ocgd_SurfaceUtils.hxx
 *
 * Godot GDExtension wrapper header for OpenCASCADE surface utility functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#ifndef OCGD_SURFACE_UTILS_HXX
#define OCGD_SURFACE_UTILS_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include "ocgd_TopoDS_Shape.hxx"

#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Dir.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>

using namespace godot;

/**
 * @brief Surface utility class for computing normals and surface properties.
 *
 * This class provides methods for computing surface normals at specific UV coordinates,
 * extracting surface properties, and performing surface-related calculations using
 * OpenCASCADE's robust geometric algorithms.
 */
class ocgd_SurfaceUtils : public RefCounted {
    GDCLASS(ocgd_SurfaceUtils, RefCounted)

public:
    /**
     * @brief Default constructor.
     */
    ocgd_SurfaceUtils();

    /**
     * @brief Destructor.
     */
    ~ocgd_SurfaceUtils();

    /**
     * @brief Compute surface normal at given UV coordinates.
     * @param face The face to compute normal for
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return Vector3 representing the surface normal direction
     */
    Vector3 compute_surface_normal(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const;

    /**
     * @brief Compute surface normal axis at given UV coordinates.
     * @param face The face to compute normal for
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return Dictionary containing 'point' and 'direction' keys
     */
    Dictionary compute_surface_normal_axis(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const;

    /**
     * @brief Get UV bounds for a face.
     * @param face The face to get bounds for
     * @return Dictionary with 'u_min', 'u_max', 'v_min', 'v_max' keys
     */
    Dictionary get_uv_bounds(const Ref<ocgd_TopoDS_Shape>& face) const;

    /**
     * @brief Compute surface point at given UV coordinates.
     * @param face The face to compute point for
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return Vector3 representing the surface point
     */
    Vector3 compute_surface_point(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const;

    /**
     * @brief Compute curvature information at given UV coordinates.
     * @param face The face to compute curvature for
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return Dictionary containing curvature information
     */
    Dictionary compute_surface_curvature(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const;

    /**
     * @brief Check if the surface normal is well-defined at UV coordinates.
     * @param face The face to check
     * @param u U parameter coordinate
     * @param v V parameter coordinate
     * @return true if normal is well-defined, false otherwise
     */
    bool is_normal_defined(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const;

    /**
     * @brief Sample surface normals at multiple UV coordinates.
     * @param face The face to sample
     * @param u_samples Array of U coordinates
     * @param v_samples Array of V coordinates
     * @return Array of Vector3 normals
     */
    Array sample_surface_normals(const Ref<ocgd_TopoDS_Shape>& face, const Array& u_samples, const Array& v_samples) const;

    /**
     * @brief Get surface type classification.
     * @param face The face to classify
     * @return String describing the surface type
     */
    String get_surface_type(const Ref<ocgd_TopoDS_Shape>& face) const;

    /**
     * @brief Set tolerance for surface computations.
     * @param tolerance The tolerance value
     */
    void set_tolerance(double tolerance);

    /**
     * @brief Get current tolerance value.
     * @return Current tolerance
     */
    double get_tolerance() const;

    /**
     * @brief Set whether to flip normals for reversed faces (static setting).
     * @param flip_for_reversed If true, normals are flipped for TopAbs_REVERSED faces
     */
    static void set_flip_normals_for_reversed(bool flip_for_reversed);

    /**
     * @brief Get whether normals are flipped for reversed faces (static setting).
     * @return Current setting for normal flipping
     */
    static bool get_flip_normals_for_reversed();

protected:
    static void _bind_methods();

private:
    /**
     * @brief Convert a TopoDS_Shape reference to TopoDS_Face.
     * @param shape The shape reference
     * @return TopoDS_Face if conversion is successful
     * @throws Standard_Failure if shape is not a face
     */
    TopoDS_Face shape_to_face(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Convert gp_Pnt to Vector3.
     * @param point OpenCASCADE point
     * @return Godot Vector3
     */
    Vector3 gp_pnt_to_vector3(const gp_Pnt& point) const;

    /**
     * @brief Convert gp_Dir to Vector3.
     * @param direction OpenCASCADE direction
     * @return Godot Vector3
     */
    Vector3 gp_dir_to_vector3(const gp_Dir& direction) const;

    /**
     * @brief Convert gp_Vec to Vector3.
     * @param vector OpenCASCADE vector
     * @return Godot Vector3
     */
    Vector3 gp_vec_to_vector3(const gp_Vec& vector) const;

    double _tolerance; ///< Tolerance for geometric computations
};

#endif // OCGD_SURFACE_UTILS_HXX