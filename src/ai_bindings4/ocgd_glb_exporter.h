#ifndef OCGD_GLB_EXPORTER_H
#define OCGD_GLB_EXPORTER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;

class ocgd_shape;

class ocgd_glb_exporter : public godot::RefCounted {
    GDCLASS(ocgd_glb_exporter, godot::RefCounted)

private:
    godot::String last_error;
    double mesh_deflection;
    double angular_deflection;
    bool export_colors;
    bool export_materials;
    bool export_textures;
    bool merge_faces;
    int texture_size;
    godot::String default_material_name;
    bool mesh_optimization;
    
    // Shape-to-color and material mappings
    godot::Dictionary shape_colors;
    godot::Dictionary shape_materials;

protected:
    static void _bind_methods();

public:
    ocgd_glb_exporter();
    ~ocgd_glb_exporter();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_glb_exporter> new_exporter();

    // Main export functions
    bool export_shape(const godot::Ref<ocgd_shape>& shape, const godot::String& file_path);
    bool export_shapes(const godot::Array& shapes, const godot::String& file_path);
    bool export_assembly(const godot::Array& shapes, const godot::Array& transforms, const godot::String& file_path);
    
    // Export with custom options
    bool export_shape_with_options(const godot::Ref<ocgd_shape>& shape, const godot::String& file_path, const godot::Dictionary& options);
    bool export_assembly_with_options(const godot::Array& shapes, const godot::Array& transforms, const godot::String& file_path, const godot::Dictionary& options);
    
    // Configuration methods for mesh quality
    void set_mesh_deflection(double deflection);
    double get_mesh_deflection() const;
    
    void set_angular_deflection(double deflection);
    double get_angular_deflection() const;
    
    // Material and color export settings
    void set_export_colors(bool enable);
    bool get_export_colors() const;
    
    void set_export_materials(bool enable);
    bool get_export_materials() const;
    
    void set_export_textures(bool enable);
    bool get_export_textures() const;
    
    void set_merge_faces(bool enable);
    bool get_merge_faces() const;
    
    void set_texture_size(int size);
    int get_texture_size() const;
    
    void set_default_material_name(const godot::String& name);
    godot::String get_default_material_name() const;
    
    // Export statistics and information
    godot::Dictionary get_export_info() const;
    godot::Dictionary get_mesh_statistics() const;
    int get_exported_triangle_count() const;
    int get_exported_vertex_count() const;
    
    // Color and material management
    void add_shape_color(const godot::Ref<ocgd_shape>& shape, const godot::Vector3& color);
    void add_shape_material(const godot::Ref<ocgd_shape>& shape, const godot::Dictionary& material);
    void clear_shape_colors();
    void clear_shape_materials();
    
    // Assembly hierarchy support
    bool export_hierarchical_assembly(const godot::Dictionary& assembly_tree, const godot::String& file_path);
    godot::Dictionary create_assembly_node(const godot::String& name, const godot::Ref<ocgd_shape>& shape, const godot::Array& transform);
    
    // Advanced export options
    void set_coordinate_system_conversion(const godot::String& from_system, const godot::String& to_system);
    void set_unit_scale(double scale);
    void set_y_up_conversion(bool enable);
    
    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();
    
    // Validation and preview
    bool validate_shapes_for_export(const godot::Array& shapes) const;
    godot::Dictionary preview_export(const godot::Array& shapes);
    
    // Supported formats and features
    godot::Array get_supported_extensions() const;
    godot::Dictionary get_export_capabilities() const;
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
    
    // Mesh optimization
    void set_mesh_optimization(bool enable);
    bool get_mesh_optimization() const;
    void set_vertex_welding_tolerance(double tolerance);
    double get_vertex_welding_tolerance() const;
};

#endif // OCGD_GLB_EXPORTER_H