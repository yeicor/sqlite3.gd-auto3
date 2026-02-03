#ifndef _ocgd_ShapeAnalyzer_HeaderFile
#define _ocgd_ShapeAnalyzer_HeaderFile

/**
 * ocgd_ShapeAnalyzer.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE shape analysis and measurement tools.
 *
 * This class provides comprehensive analysis capabilities for OpenCASCADE shapes including:
 * - Global properties (mass, volume, surface area, center of mass)
 * - Geometric measurements (length, area, volume calculations)
 * - Inertia properties (moments of inertia, principal axes, radius of gyration)
 * - Bounding box and geometric bounds analysis
 * - Shape validation and quality checks
 * - Distance and proximity measurements
 * - Geometric feature detection and classification
 *
 * The analyzer supports different computation methods:
 * - Exact analytical calculations using geometry
 * - Triangulation-based approximations for performance
 * - Adaptive precision based on shape complexity
 *
 * Original OCCT headers: <opencascade/BRepGProp.hxx>, <opencascade/GProp_GProps.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/BRepGProp.hxx>
#include <opencascade/GProp_GProps.hxx>
#include <opencascade/GProp_PrincipalProps.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Mat.hxx>
#include <opencascade/Bnd_Box.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/BRepExtrema_DistShapeShape.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_ShapeAnalyzer
 *
 * Comprehensive analysis and measurement utility for OpenCASCADE shapes.
 *
 * This class provides methods to analyze geometric properties, validate shapes,
 * compute measurements, and extract geometric information from CAD models.
 * It supports both exact analytical calculations and approximation methods
 * for performance-critical applications.
 */
class ocgd_ShapeAnalyzer : public RefCounted {
    GDCLASS(ocgd_ShapeAnalyzer, RefCounted);

protected:
    static void _bind_methods();

private:
    // Analysis settings
    bool _use_triangulation;
    bool _skip_shared_edges;
    double _linear_deflection;
    double _angular_deflection;
    bool _high_precision_mode;

public:
    //! Default constructor
    ocgd_ShapeAnalyzer();

    //! Destructor
    virtual ~ocgd_ShapeAnalyzer();

    // Configuration methods

    //! Set whether to use triangulation for calculations (faster but approximate)
    void set_use_triangulation(bool use_triangulation);

    //! Get triangulation usage setting
    bool get_use_triangulation() const;

    //! Set whether to skip shared edges in linear property calculations
    void set_skip_shared_edges(bool skip_shared);

    //! Get shared edge skipping setting
    bool get_skip_shared_edges() const;

    //! Set linear deflection for approximation calculations
    void set_linear_deflection(double deflection);

    //! Get linear deflection setting
    double get_linear_deflection() const;

    //! Set angular deflection for approximation calculations
    void set_angular_deflection(double deflection);

    //! Get angular deflection setting
    double get_angular_deflection() const;

    //! Enable high precision mode for critical measurements
    void set_high_precision_mode(bool high_precision);

    //! Get high precision mode setting
    bool get_high_precision_mode() const;

    // Global Properties Analysis

    //! Compute volume and volumetric properties
    //! Returns Dictionary with: "volume", "center_of_mass", "mass"
    Dictionary compute_volume_properties(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Compute surface area and surface properties  
    //! Returns Dictionary with: "area", "center_of_mass", "mass"
    Dictionary compute_surface_properties(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Compute linear properties (edge lengths)
    //! Returns Dictionary with: "length", "center_of_mass", "mass"
    Dictionary compute_linear_properties(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get just the volume of a shape
    double get_volume(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get just the surface area of a shape
    double get_surface_area(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get just the total edge length of a shape
    double get_total_length(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get center of mass for the shape
    Vector3 get_center_of_mass(const Ref<ocgd_TopoDS_Shape>& shape);

    // Inertia and Moments Analysis

    //! Compute complete inertia properties
    //! Returns Dictionary with inertia matrix, principal axes, moments, radius of gyration
    Dictionary compute_inertia_properties(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& reference_point = Vector3(0, 0, 0));

    //! Get moment of inertia about a specific axis
    double get_moment_about_axis(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& axis_origin, const Vector3& axis_direction);

    //! Get radius of gyration about a specific axis
    double get_radius_of_gyration(const Ref<ocgd_TopoDS_Shape>& shape, const Vector3& axis_origin, const Vector3& axis_direction);

    //! Get principal moments of inertia
    //! Returns Array of three principal moment values
    Array get_principal_moments(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get principal axes of inertia
    //! Returns Array of three Vector3 direction vectors
    Array get_principal_axes(const Ref<ocgd_TopoDS_Shape>& shape);

    // Bounding and Geometric Analysis

    //! Get axis-aligned bounding box
    //! Returns Dictionary with "min" and "max" Vector3 coordinates
    Dictionary get_bounding_box(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get oriented bounding box (minimal volume enclosing box)
    //! Returns Dictionary with "center", "axes", "half_extents"
    Dictionary get_oriented_bounding_box(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get bounding sphere (minimal sphere enclosing the shape)
    //! Returns Dictionary with "center" and "radius"
    Dictionary get_bounding_sphere(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get shape dimensions (length, width, height of bounding box)
    Vector3 get_dimensions(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get maximum dimension of the shape
    double get_max_dimension(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get minimum dimension of the shape
    double get_min_dimension(const Ref<ocgd_TopoDS_Shape>& shape);

    // Shape Validation and Quality

    //! Validate shape for geometric consistency
    //! Returns Dictionary with "is_valid", "errors", "warnings"
    Dictionary validate_shape(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Check if shape is a valid solid
    bool is_valid_solid(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Check if shape forms a closed shell
    bool is_closed_shell(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Check if shape is manifold (no edge shared by more than 2 faces)
    bool is_manifold(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get shape quality metrics
    //! Returns Dictionary with various quality indicators
    Dictionary get_quality_metrics(const Ref<ocgd_TopoDS_Shape>& shape);

    // Distance and Proximity Analysis

    //! Compute minimum distance between two shapes
    double get_distance_between_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Get closest points between two shapes
    //! Returns Dictionary with "point1", "point2", "distance"
    Dictionary get_closest_points(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Check if two shapes intersect
    bool shapes_intersect(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Get distance from point to shape
    double get_distance_point_to_shape(const Vector3& point, const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get closest point on shape to given point
    Vector3 get_closest_point_on_shape(const Vector3& point, const Ref<ocgd_TopoDS_Shape>& shape);

    // Feature Analysis

    //! Count different types of topological entities
    //! Returns Dictionary with counts for vertices, edges, faces, solids, etc.
    Dictionary get_topology_counts(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Analyze shape complexity
    //! Returns Dictionary with complexity metrics
    Dictionary analyze_complexity(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Detect geometric features (holes, pockets, bosses, etc.)
    //! Returns Array of feature descriptions
    Array detect_features(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Get curvature analysis for surfaces
    //! Returns Dictionary with curvature statistics
    Dictionary analyze_curvature(const Ref<ocgd_TopoDS_Shape>& shape);

    // Comparison and Similarity

    //! Compare two shapes for geometric similarity
    //! Returns Dictionary with similarity metrics
    Dictionary compare_shapes(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2);

    //! Check if two shapes are geometrically identical within tolerance
    bool shapes_are_identical(const Ref<ocgd_TopoDS_Shape>& shape1, const Ref<ocgd_TopoDS_Shape>& shape2, double tolerance = 1e-6);

    //! Get shape signature for comparison purposes
    String get_shape_signature(const Ref<ocgd_TopoDS_Shape>& shape);

    // Batch Analysis

    //! Analyze multiple shapes and return summary statistics
    Dictionary analyze_multiple_shapes(const Array& shapes);

    //! Find shapes within tolerance of target properties
    Array find_shapes_by_properties(const Array& shapes, const Dictionary& target_properties, double tolerance = 0.1);

    // Utility Methods

    //! Get comprehensive analysis report for a shape
    Dictionary get_full_analysis_report(const Ref<ocgd_TopoDS_Shape>& shape);

    //! Export analysis results to formatted string
    String export_analysis_to_string(const Dictionary& analysis_data, const String& format = "human_readable");

    //! Get recommended mesh parameters based on shape analysis
    Dictionary get_recommended_mesh_parameters(const Ref<ocgd_TopoDS_Shape>& shape);

private:
    //! Internal helper to convert GProp_GProps to Dictionary
    Dictionary gprops_to_dictionary(const GProp_GProps& props) const;

    //! Internal helper to compute oriented bounding box
    Dictionary compute_obb(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Internal helper for shape validation
    Dictionary perform_shape_check(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Internal helper for feature detection algorithms
    Array detect_geometric_features(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Internal helper for curvature analysis
    Dictionary compute_curvature_stats(const Ref<ocgd_TopoDS_Shape>& shape) const;
};

#endif // _ocgd_ShapeAnalyzer_HeaderFile