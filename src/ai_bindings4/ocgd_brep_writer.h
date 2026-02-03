#ifndef OCGD_BREP_WRITER_H
#define OCGD_BREP_WRITER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;

class ocgd_shape;

class ocgd_brep_writer : public godot::RefCounted {
    GDCLASS(ocgd_brep_writer, godot::RefCounted)

private:
    godot::String last_error;
    bool write_triangulation;
    bool write_curves;
    bool write_surfaces;
    double precision_tolerance;
    godot::String brep_version;
    bool binary_format;
    bool compress_output;
    bool shape_optimization;

protected:
    static void _bind_methods();

public:
    ocgd_brep_writer();
    ~ocgd_brep_writer();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_brep_writer> new_writer();

    // Main export functions
    bool write_file(const godot::Ref<ocgd_shape>& shape, const godot::String& file_path);
    bool write_shapes(const godot::Array& shapes, const godot::String& file_path);
    
    // Export with custom options
    bool write_file_with_options(const godot::Ref<ocgd_shape>& shape, const godot::String& file_path, const godot::Dictionary& options);
    bool write_shapes_with_options(const godot::Array& shapes, const godot::String& file_path, const godot::Dictionary& options);
    
    // Export to string content
    godot::String write_to_string(const godot::Ref<ocgd_shape>& shape);
    godot::String write_shapes_to_string(const godot::Array& shapes);
    
    // Configuration methods
    void set_write_triangulation(bool enable);
    bool get_write_triangulation() const;
    
    void set_write_curves(bool enable);
    bool get_write_curves() const;
    
    void set_write_surfaces(bool enable);
    bool get_write_surfaces() const;
    
    void set_precision_tolerance(double tolerance);
    double get_precision_tolerance() const;
    
    void set_brep_version(const godot::String& version);
    godot::String get_brep_version() const;
    
    void set_binary_format(bool binary);
    bool get_binary_format() const;
    
    void set_compress_output(bool compress);
    bool get_compress_output() const;
    
    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();
    
    // Validation
    bool validate_shape_for_export(const godot::Ref<ocgd_shape>& shape);
    bool validate_shapes_for_export(const godot::Array& shapes);
    
    // Get supported file extensions
    godot::Array get_supported_extensions() const;
    
    // Export information and statistics
    godot::Dictionary get_export_info() const;
    godot::Dictionary get_export_statistics() const;
    int get_exported_shape_count() const;
    double get_export_file_size() const;
    
    // Format detection and conversion
    bool can_export_shape_type(const godot::String& shape_type);
    godot::Array get_supported_shape_types() const;
    
    // Advanced export options
    void set_shape_optimization(bool enable);
    bool get_shape_optimization() const;
    
    void set_remove_duplicates(bool enable);
    bool get_remove_duplicates() const;
    
    void set_merge_vertices(bool enable);
    bool get_merge_vertices() const;
    
    void set_fix_shapes_before_export(bool enable);
    bool get_fix_shapes_before_export() const;
    
    // Quality and validation
    bool perform_shape_check_before_export(const godot::Ref<ocgd_shape>& shape);
    godot::Dictionary get_shape_quality_report(const godot::Ref<ocgd_shape>& shape);
    bool repair_shape_for_export(godot::Ref<ocgd_shape>& shape);
    
    // Batch export operations
    bool write_multiple_files(const godot::Array& shapes, const godot::Array& file_paths);
    bool write_assembly_as_separate_files(const godot::Ref<ocgd_shape>& assembly, const godot::String& base_path);
    
    // Metadata and documentation
    void set_export_metadata(const godot::Dictionary& metadata);
    godot::Dictionary get_export_metadata() const;
    void add_export_comment(const godot::String& comment);
    void clear_export_comments();
    
    // Progress tracking
    void set_progress_callback_enabled(bool enable);
    bool get_progress_callback_enabled() const;
    double get_export_progress() const;
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
    
    // Compatibility and versioning
    godot::Array get_supported_brep_versions() const;
    bool is_version_compatible(const godot::String& version) const;
    godot::String get_recommended_version() const;
    
    // Performance optimization
    void set_memory_limit(int limit_mb);
    int get_memory_limit() const;
    void set_streaming_mode(bool enable);
    bool get_streaming_mode() const;
};

#endif // OCGD_BREP_WRITER_H