#ifndef OCGD_SHAPE_H
#define OCGD_SHAPE_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;

class ocgd_shape : public godot::RefCounted {
    GDCLASS(ocgd_shape, godot::RefCounted)

private:
    TopoDS_Shape* occ_shape;
    bool owns_shape;

protected:
    static void _bind_methods();

public:
    ocgd_shape();
    ~ocgd_shape();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_shape> new_shape();

    // Shape management
    bool has_shape() const;
    void clear_shape();

    // Shape information
    godot::String get_shape_type() const;
    int get_num_faces() const;
    int get_num_edges() const;
    int get_num_vertices() const;
    
    // Geometric properties
    double get_volume() const;
    double get_surface_area() const;
    godot::Vector3 get_center_of_mass() const;
    godot::Array get_bounding_box() const; // Returns [min_point, max_point]
    
    // Shape validation
    bool is_valid() const;
    bool is_closed() const;
    bool is_infinite() const;
    
    // Shape operations
    godot::Ref<ocgd_shape> copy() const;
    void transform_translate(const godot::Vector3& translation);
    void transform_rotate(const godot::Vector3& axis, double angle);
    void transform_scale(double factor);
    void transform_scale_xyz(const godot::Vector3& scale);
    
    // Topology exploration
    godot::Array get_faces() const;
    godot::Array get_edges() const;
    godot::Array get_vertices() const;
    godot::Array get_subshapes(const godot::String& shape_type) const;
    
    // Measurement utilities
    double distance_to_shape(const godot::Ref<ocgd_shape>& other) const;
    double distance_to_point(const godot::Vector3& point) const;
    godot::Vector3 closest_point_to(const godot::Vector3& point) const;
    
    // Boolean operations
    godot::Ref<ocgd_shape> fuse_with(const godot::Ref<ocgd_shape>& other) const;
    godot::Ref<ocgd_shape> cut_with(const godot::Ref<ocgd_shape>& other) const;
    godot::Ref<ocgd_shape> common_with(const godot::Ref<ocgd_shape>& other) const;
    
    // Export functionality
    bool export_step(const godot::String& file_path) const;
    bool export_iges(const godot::String& file_path) const;
    bool export_brep(const godot::String& file_path) const;
    bool export_stl(const godot::String& file_path) const;
    
    // Mesh extraction for Godot
    godot::Dictionary extract_mesh_data() const;
    godot::Array extract_triangulated_mesh() const;
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
    godot::Dictionary get_shape_info() const;
    
    // Hash and comparison
    int get_hash_code() const;
    bool is_same_as(const godot::Ref<ocgd_shape>& other) const;
    bool is_equal_to(const godot::Ref<ocgd_shape>& other) const;
    
    // Internal methods for C++ usage
    void _set_shape_internal(const TopoDS_Shape& shape, bool take_ownership = true);
    const TopoDS_Shape* _get_shape_ptr() const;
    TopoDS_Shape get_shape() const;
    void set_shape(const TopoDS_Shape& shape);
};

#endif // OCGD_SHAPE_H