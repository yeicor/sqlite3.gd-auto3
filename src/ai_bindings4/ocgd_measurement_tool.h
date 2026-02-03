#ifndef OCGD_MEASUREMENT_TOOL_H
#define OCGD_MEASUREMENT_TOOL_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Vertex;

class ocgd_shape;

class ocgd_measurement_tool : public godot::RefCounted {
    GDCLASS(ocgd_measurement_tool, godot::RefCounted)

private:
    godot::String last_error;
    double precision_tolerance;
    bool use_high_precision;
    bool include_detailed_info;
    godot::String measurement_units;
    double unit_scale_factor;
    bool validate_inputs;

protected:
    static void _bind_methods();

public:
    ocgd_measurement_tool();
    ~ocgd_measurement_tool();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_measurement_tool> new_tool();

    // Basic distance measurements
    double measure_distance_3d(const godot::Vector3& point1, const godot::Vector3& point2);
    double measure_distance_to_shape(const godot::Vector3& point, const godot::Ref<ocgd_shape>& shape);
    double measure_distance_between_shapes(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    
    // Advanced distance measurements with detailed results
    godot::Dictionary measure_distance_detailed(const godot::Vector3& point1, const godot::Vector3& point2);
    godot::Dictionary measure_closest_points(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    godot::Dictionary measure_minimum_distance_analysis(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    
    // Angular measurements
    double measure_angle_between_vectors(const godot::Vector3& vector1, const godot::Vector3& vector2);
    double measure_angle_between_planes(const godot::Ref<ocgd_shape>& plane1, const godot::Ref<ocgd_shape>& plane2);
    double measure_angle_between_edges(const godot::Ref<ocgd_shape>& shape, int edge_index1, int edge_index2);
    godot::Dictionary measure_dihedral_angle(const godot::Ref<ocgd_shape>& shape, int face_index1, int face_index2);
    
    // Surface and volume measurements
    double measure_surface_area(const godot::Ref<ocgd_shape>& shape);
    double measure_volume(const godot::Ref<ocgd_shape>& shape);
    double measure_face_area(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_edge_length(const godot::Ref<ocgd_shape>& shape, int edge_index);
    double measure_wire_length(const godot::Ref<ocgd_shape>& shape, int wire_index);
    
    // Perimeter and circumference measurements
    double measure_perimeter(const godot::Ref<ocgd_shape>& shape);
    double measure_face_perimeter(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_circumference(const godot::Ref<ocgd_shape>& shape);
    
    // Geometric property measurements
    godot::Vector3 measure_center_of_mass(const godot::Ref<ocgd_shape>& shape);
    godot::Vector3 measure_centroid(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary measure_moments_of_inertia(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary measure_principal_axes(const godot::Ref<ocgd_shape>& shape);
    
    // Bounding measurements
    godot::Dictionary measure_bounding_box(const godot::Ref<ocgd_shape>& shape);
    double measure_bounding_box_volume(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary measure_oriented_bounding_box(const godot::Ref<ocgd_shape>& shape);
    double measure_bounding_sphere_radius(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary measure_bounding_sphere(const godot::Ref<ocgd_shape>& shape);
    
    // Curvature measurements
    double measure_curvature_at_point(const godot::Ref<ocgd_shape>& shape, int face_index, double u, double v);
    godot::Dictionary measure_curvature_analysis(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_gaussian_curvature(const godot::Ref<ocgd_shape>& shape, int face_index, double u, double v);
    double measure_mean_curvature(const godot::Ref<ocgd_shape>& shape, int face_index, double u, double v);
    godot::Dictionary measure_principal_curvatures(const godot::Ref<ocgd_shape>& shape, int face_index, double u, double v);
    
    // Specialized CAD measurements
    double measure_hole_diameter(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_fillet_radius(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_chamfer_distance(const godot::Ref<ocgd_shape>& shape, int face_index);
    godot::Dictionary measure_thread_pitch(const godot::Ref<ocgd_shape>& shape);
    
    // Thickness and wall measurements
    double measure_wall_thickness(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& point, const godot::Vector3& direction);
    godot::Dictionary measure_thickness_analysis(const godot::Ref<ocgd_shape>& shape);
    double measure_minimum_wall_thickness(const godot::Ref<ocgd_shape>& shape);
    godot::Array measure_thickness_variations(const godot::Ref<ocgd_shape>& shape, int num_samples);
    
    // Cross-sectional measurements
    godot::Dictionary measure_cross_section_area(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& plane_point, const godot::Vector3& plane_normal);
    godot::Dictionary measure_cross_section_properties(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& plane_point, const godot::Vector3& plane_normal);
    double measure_hydraulic_diameter(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& plane_point, const godot::Vector3& plane_normal);
    
    // Draft and taper measurements
    double measure_draft_angle(const godot::Ref<ocgd_shape>& shape, int face_index, const godot::Vector3& draft_direction);
    godot::Dictionary measure_taper_analysis(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& axis_direction);
    godot::Array measure_undercuts(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& draft_direction);
    
    // Clearance and interference measurements
    double measure_clearance(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    godot::Dictionary measure_interference_analysis(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    godot::Array measure_interference_points(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    double measure_penetration_depth(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    
    // Assembly and fit measurements
    godot::Dictionary measure_fit_analysis(const godot::Ref<ocgd_shape>& hole_shape, const godot::Ref<ocgd_shape>& shaft_shape);
    double measure_concentricity_error(const godot::Ref<ocgd_shape>& shape1, const godot::Ref<ocgd_shape>& shape2);
    double measure_parallelism_error(const godot::Ref<ocgd_shape>& shape, int face_index1, int face_index2);
    double measure_perpendicularity_error(const godot::Ref<ocgd_shape>& shape, int face_index1, int face_index2);
    double measure_flatness_error(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_cylindricity_error(const godot::Ref<ocgd_shape>& shape, int face_index);
    double measure_sphericity_error(const godot::Ref<ocgd_shape>& shape, int face_index);
    
    // Coordinate system measurements
    godot::Dictionary measure_in_coordinate_system(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& origin, const godot::Vector3& x_axis, const godot::Vector3& y_axis);
    godot::Vector3 transform_to_coordinate_system(const godot::Vector3& point, const godot::Vector3& origin, const godot::Vector3& x_axis, const godot::Vector3& y_axis);
    godot::Vector3 transform_from_coordinate_system(const godot::Vector3& point, const godot::Vector3& origin, const godot::Vector3& x_axis, const godot::Vector3& y_axis);
    
    // Statistical measurements
    godot::Dictionary measure_statistical_analysis(const godot::Array& measurements);
    double measure_standard_deviation(const godot::Array& measurements);
    double measure_variance(const godot::Array& measurements);
    godot::Dictionary measure_distribution_analysis(const godot::Array& measurements);
    
    // Batch measurements
    godot::Array measure_multiple_distances(const godot::Array& point_pairs);
    godot::Array measure_multiple_areas(const godot::Ref<ocgd_shape>& shape, const godot::Array& face_indices);
    godot::Array measure_multiple_volumes(const godot::Array& shapes);
    godot::Dictionary measure_comparative_analysis(const godot::Array& shapes);
    
    // Configuration methods
    void set_precision_tolerance(double tolerance);
    double get_precision_tolerance() const;
    
    void set_use_high_precision(bool high_precision);
    bool get_use_high_precision() const;
    
    void set_include_detailed_info(bool detailed);
    bool get_include_detailed_info() const;
    
    void set_measurement_units(const godot::String& units);
    godot::String get_measurement_units() const;
    
    void set_unit_scale_factor(double scale);
    double get_unit_scale_factor() const;
    
    void set_validate_inputs(bool validate);
    bool get_validate_inputs() const;
    
    // Utility and validation methods
    bool validate_shape(const godot::Ref<ocgd_shape>& shape);
    bool validate_face_index(const godot::Ref<ocgd_shape>& shape, int face_index);
    bool validate_edge_index(const godot::Ref<ocgd_shape>& shape, int edge_index);
    bool validate_measurement_parameters(const godot::Dictionary& parameters);
    
    // Unit conversion methods
    double convert_length(double value, const godot::String& from_units, const godot::String& to_units);
    double convert_area(double value, const godot::String& from_units, const godot::String& to_units);
    double convert_volume(double value, const godot::String& from_units, const godot::String& to_units);
    double convert_angle(double value, const godot::String& from_units, const godot::String& to_units);
    
    // Export and reporting methods
    godot::Dictionary export_measurement_report(const godot::Dictionary& measurements);
    godot::String generate_measurement_summary(const godot::Dictionary& measurements);
    bool save_measurement_report(const godot::Dictionary& measurements, const godot::String& file_path);
    godot::Array get_supported_units() const;
    
    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
    
    // Advanced measurement algorithms
    godot::Dictionary measure_form_factor(const godot::Ref<ocgd_shape>& shape);
    double measure_aspect_ratio(const godot::Ref<ocgd_shape>& shape);
    double measure_compactness(const godot::Ref<ocgd_shape>& shape);
    double measure_sphericity(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary measure_shape_complexity(const godot::Ref<ocgd_shape>& shape);
    
    // Quality assurance measurements
    godot::Dictionary measure_geometric_tolerance_analysis(const godot::Ref<ocgd_shape>& shape);
    godot::Array measure_feature_consistency(const godot::Array& shapes);
    godot::Dictionary measure_manufacturing_feasibility(const godot::Ref<ocgd_shape>& shape);
    double measure_surface_roughness_estimate(const godot::Ref<ocgd_shape>& shape, int face_index);
};

#endif // OCGD_MEASUREMENT_TOOL_H