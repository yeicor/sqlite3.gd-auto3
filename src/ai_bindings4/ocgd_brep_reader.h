#ifndef OCGD_BREP_READER_H
#define OCGD_BREP_READER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;

class ocgd_shape;

class ocgd_brep_reader : public godot::RefCounted {
    GDCLASS(ocgd_brep_reader, godot::RefCounted)

private:
    godot::String last_error;
    bool load_triangulation;
    bool load_curves;
    bool load_surfaces;
    double precision_tolerance;
    bool merge_vertices;
    bool fix_shapes;
    int memory_limit;
    double load_time;
    int memory_used;
    int shapes_loaded;

protected:
    static void _bind_methods();

public:
    ocgd_brep_reader();
    ~ocgd_brep_reader();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_brep_reader> new_reader();

    // Main import function
    godot::Ref<ocgd_shape> load_file(const godot::String& file_path);
    
    // Import with additional options
    godot::Ref<ocgd_shape> load_file_with_options(const godot::String& file_path, const godot::Dictionary& options);
    
    // Load from string content
    godot::Ref<ocgd_shape> load_from_string(const godot::String& brep_content);
    
    // Get detailed information about the loaded file
    godot::Dictionary get_file_info() const;
    
    // Get list of all shapes in the file
    godot::Array get_all_shapes();
    
    // Configuration methods
    void set_load_triangulation(bool enable);
    bool get_load_triangulation() const;
    
    void set_load_curves(bool enable);
    bool get_load_curves() const;
    
    void set_load_surfaces(bool enable);
    bool get_load_surfaces() const;
    
    void set_precision_tolerance(double tolerance);
    double get_precision_tolerance() const;
    
    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();
    
    // Validation
    bool validate_file(const godot::String& file_path);
    bool validate_string(const godot::String& brep_content);
    
    // Get supported file extensions
    godot::Array get_supported_extensions() const;
    
    // Shape analysis
    godot::Dictionary analyze_shape(const godot::Ref<ocgd_shape>& shape) const;
    int get_shape_complexity(const godot::Ref<ocgd_shape>& shape) const;
    
    // Format detection
    bool is_brep_file(const godot::String& file_path) const;
    godot::String detect_brep_version(const godot::String& file_path) const;
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
    
    // Advanced loading options
    void set_merge_vertices(bool enable);
    bool get_merge_vertices() const;
    
    void set_fix_shapes(bool enable);
    bool get_fix_shapes() const;
    
    // Memory and performance
    void set_memory_limit(int limit_mb);
    int get_memory_limit() const;
    
    // Statistics
    godot::Dictionary get_load_statistics() const;
    double get_load_time() const;
};

#endif // OCGD_BREP_READER_H