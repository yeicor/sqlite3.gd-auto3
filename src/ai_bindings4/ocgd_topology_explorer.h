#ifndef OCGD_TOPOLOGY_EXPLORER_H
#define OCGD_TOPOLOGY_EXPLORER_H

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

class ocgd_topology_explorer : public godot::RefCounted {
    GDCLASS(ocgd_topology_explorer, godot::RefCounted)

private:
    godot::String last_error;
    godot::Ref<ocgd_shape> current_shape;
    double precision_tolerance;
    bool include_orientation_info;
    bool cache_results;

protected:
    static void _bind_methods();

public:
    ocgd_topology_explorer();
    ~ocgd_topology_explorer();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_topology_explorer> new_explorer();

    // Shape assignment and management
    void set_shape(const godot::Ref<ocgd_shape>& shape);
    godot::Ref<ocgd_shape> get_shape() const;
    bool has_shape() const;
    void clear_shape();

    // Basic topology exploration
    godot::Array get_faces();
    godot::Array get_edges();
    godot::Array get_vertices();
    godot::Array get_solids();
    godot::Array get_shells();
    godot::Array get_wires();
    godot::Array get_compounds();

    // Counting methods
    int get_face_count() const;
    int get_edge_count() const;
    int get_vertex_count() const;
    int get_solid_count() const;
    int get_shell_count() const;
    int get_wire_count() const;
    int get_compound_count() const;

    // Shape hierarchy and relationships
    godot::Array get_faces_of_solid(int solid_index);
    godot::Array get_edges_of_face(int face_index);
    godot::Array get_vertices_of_edge(int edge_index);
    godot::Array get_edges_of_vertex(int vertex_index);
    godot::Array get_adjacent_faces(int face_index);
    godot::Array get_connected_edges(int edge_index);

    // Geometric properties of faces
    godot::Dictionary get_face_properties(int face_index);
    godot::Vector3 get_face_center(int face_index);
    godot::Vector3 get_face_normal(int face_index);
    double get_face_area(int face_index);
    godot::Dictionary get_face_bounds(int face_index);
    godot::String get_face_type(int face_index);
    bool is_face_planar(int face_index);
    bool is_face_closed(int face_index);

    // Geometric properties of edges
    godot::Dictionary get_edge_properties(int edge_index);
    godot::Vector3 get_edge_start_point(int edge_index);
    godot::Vector3 get_edge_end_point(int edge_index);
    godot::Vector3 get_edge_center(int edge_index);
    double get_edge_length(int edge_index);
    godot::Dictionary get_edge_bounds(int edge_index);
    godot::String get_edge_type(int edge_index);
    bool is_edge_straight(int edge_index);
    bool is_edge_closed(int edge_index);
    double get_edge_curvature_at_param(int edge_index, double parameter);

    // Geometric properties of vertices
    godot::Dictionary get_vertex_properties(int vertex_index);
    godot::Vector3 get_vertex_position(int vertex_index);
    double get_vertex_tolerance(int vertex_index);

    // Distance measurements
    double measure_distance_point_to_point(const godot::Vector3& point1, const godot::Vector3& point2);
    double measure_distance_point_to_face(const godot::Vector3& point, int face_index);
    double measure_distance_point_to_edge(const godot::Vector3& point, int edge_index);
    double measure_distance_face_to_face(int face_index1, int face_index2);
    double measure_distance_edge_to_edge(int edge_index1, int edge_index2);
    double measure_distance_vertex_to_vertex(int vertex_index1, int vertex_index2);

    // Advanced distance analysis
    godot::Dictionary get_closest_points_face_to_face(int face_index1, int face_index2);
    godot::Dictionary get_closest_points_edge_to_edge(int edge_index1, int edge_index2);
    godot::Vector3 get_closest_point_on_face(const godot::Vector3& point, int face_index);
    godot::Vector3 get_closest_point_on_edge(const godot::Vector3& point, int edge_index);

    // Shape analysis
    godot::Dictionary analyze_shape_topology();
    godot::Dictionary get_shape_statistics();
    bool is_shape_valid();
    bool is_shape_closed();
    bool is_shape_manifold();
    godot::Array find_free_edges();
    godot::Array find_degenerate_faces();
    godot::Array find_small_features(double min_size);

    // Curvature analysis
    godot::Dictionary get_face_curvature_at_uv(int face_index, double u, double v);
    double get_face_gaussian_curvature_at_uv(int face_index, double u, double v);
    double get_face_mean_curvature_at_uv(int face_index, double u, double v);
    godot::Dictionary get_edge_curvature_analysis(int edge_index);

    // UV parameter analysis for faces
    godot::Dictionary get_face_uv_bounds(int face_index);
    godot::Vector3 evaluate_face_at_uv(int face_index, double u, double v);
    godot::Dictionary get_face_derivatives_at_uv(int face_index, double u, double v);

    // Intersection analysis
    godot::Array find_face_intersections(int face_index1, int face_index2);
    godot::Array find_edge_intersections(int edge_index1, int edge_index2);
    bool faces_intersect(int face_index1, int face_index2);
    bool edges_intersect(int edge_index1, int edge_index2);

    // Bounding box and spatial analysis
    godot::Dictionary get_overall_bounding_box();
    godot::Dictionary get_face_bounding_box(int face_index);
    godot::Dictionary get_edge_bounding_box(int edge_index);
    godot::Dictionary get_vertex_bounding_box(int vertex_index);

    // Mass properties (requires solids)
    godot::Dictionary calculate_mass_properties();
    godot::Vector3 get_center_of_gravity();
    godot::Dictionary get_moments_of_inertia();
    double get_volume();
    double get_surface_area();

    // Feature recognition
    godot::Array find_holes();
    godot::Array find_fillets();
    godot::Array find_chamfers();
    godot::Array find_cylindrical_faces();
    godot::Array find_planar_faces();
    godot::Array find_spherical_faces();
    godot::Array find_conical_faces();

    // Configuration methods
    void set_precision_tolerance(double tolerance);
    double get_precision_tolerance() const;
    
    void set_include_orientation_info(bool include);
    bool get_include_orientation_info() const;
    
    void set_cache_results(bool cache);
    bool get_cache_results() const;

    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();

    // Utility methods
    godot::String get_type() const;
    bool is_null() const;

    // Export topology information
    godot::Dictionary export_topology_info();
    godot::String export_topology_report();
    bool save_topology_analysis(const godot::String& file_path);

    // Comparison methods
    bool compare_topologies(const godot::Ref<ocgd_shape>& other_shape);
    godot::Dictionary get_topology_differences(const godot::Ref<ocgd_shape>& other_shape);
    double calculate_shape_similarity(const godot::Ref<ocgd_shape>& other_shape);
};

#endif // OCGD_TOPOLOGY_EXPLORER_H