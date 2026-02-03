#ifndef OCGD_STEP_READER_H
#define OCGD_STEP_READER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/ref.hpp>

// Forward declarations for OpenCASCADE
class TopoDS_Shape;

class ocgd_shape;

class ocgd_step_reader : public godot::RefCounted {
    GDCLASS(ocgd_step_reader, godot::RefCounted)

private:
    godot::String last_error;
    bool transfer_colors;
    bool transfer_names;
    bool transfer_layers;

protected:
    static void _bind_methods();

public:
    ocgd_step_reader();
    ~ocgd_step_reader();

    // Factory method since GDExtensions don't support non-empty constructors
    static godot::Ref<ocgd_step_reader> new_reader();

    // Main import function
    godot::Ref<ocgd_shape> load_file(const godot::String& file_path);
    
    // Import with additional options
    godot::Ref<ocgd_shape> load_file_with_options(const godot::String& file_path, const godot::Dictionary& options);
    
    // Get detailed information about the loaded file
    godot::Dictionary get_file_info() const;
    
    // Get list of all shapes/assemblies in the file
    godot::Array get_all_shapes();
    
    // Configuration methods
    void set_transfer_colors(bool enable);
    bool get_transfer_colors() const;
    
    void set_transfer_names(bool enable);
    bool get_transfer_names() const;
    
    void set_transfer_layers(bool enable);
    bool get_transfer_layers() const;
    
    // Error handling
    godot::String get_last_error() const;
    bool has_error() const;
    void clear_error();
    
    // Validation
    bool validate_file(const godot::String& file_path);
    
    // Get supported file extensions
    godot::Array get_supported_extensions() const;
    
    // Utility methods
    godot::String get_type() const;
    bool is_null() const;
};

#endif // OCGD_STEP_READER_H