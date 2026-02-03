#ifndef OCGD_STEP_WRITER_H
#define OCGD_STEP_WRITER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include "ocgd_shape.h"

namespace godot {

class ocgd_step_writer : public RefCounted {
    GDCLASS(ocgd_step_writer, RefCounted)

private:
    String last_error;
    String application_protocol;
    String schema_identifier;
    String originating_system;
    String authorization;
    String description;
    String implementation_level;
    double length_unit;
    double angle_unit;
    double solid_angle_unit;
    bool write_colors;
    bool write_materials;
    bool write_layers;
    bool write_names;
    bool validate_on_write;
    bool write_precision_mode;
    double precision_value;
    bool merge_surfaces;
    bool write_tessellation;
    double tessellation_deviation;
    double tessellation_angular_deflection;

protected:
    static void _bind_methods();

public:
    ocgd_step_writer();
    ~ocgd_step_writer();

    // Factory method
    static Ref<ocgd_step_writer> new_step_writer();

    // Core writing functionality
    bool write_shape(const Ref<ocgd_shape>& shape, const String& file_path);
    bool write_shapes(const Array& shapes, const String& file_path);

    // Configuration methods
    void set_application_protocol(const String& protocol);
    String get_application_protocol() const;

    void set_schema_identifier(const String& schema);
    String get_schema_identifier() const;

    void set_originating_system(const String& system);
    String get_originating_system() const;

    void set_authorization(const String& auth);
    String get_authorization() const;

    void set_description(const String& desc);
    String get_description() const;

    void set_implementation_level(const String& level);
    String get_implementation_level() const;

    // Unit settings
    void set_length_unit(double unit);
    double get_length_unit() const;

    void set_angle_unit(double unit);
    double get_angle_unit() const;

    void set_solid_angle_unit(double unit);
    double get_solid_angle_unit() const;

    // Feature control
    void set_write_colors(bool enable);
    bool get_write_colors() const;

    void set_write_materials(bool enable);
    bool get_write_materials() const;

    void set_write_layers(bool enable);
    bool get_write_layers() const;

    void set_write_names(bool enable);
    bool get_write_names() const;

    void set_validate_on_write(bool enable);
    bool get_validate_on_write() const;

    // Precision control
    void set_write_precision_mode(bool enable);
    bool get_write_precision_mode() const;

    void set_precision_value(double precision);
    double get_precision_value() const;

    // Surface options
    void set_merge_surfaces(bool enable);
    bool get_merge_surfaces() const;

    // Tessellation options
    void set_write_tessellation(bool enable);
    bool get_write_tessellation() const;

    void set_tessellation_deviation(double deviation);
    double get_tessellation_deviation() const;

    void set_tessellation_angular_deflection(double deflection);
    double get_tessellation_angular_deflection() const;

    // Advanced writing methods
    bool write_assembly(const Dictionary& assembly_structure, const String& file_path);
    bool write_with_metadata(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& metadata);
    bool write_shapes_with_colors(const Array& shapes, const Array& colors, const String& file_path);
    bool write_shapes_with_materials(const Array& shapes, const Array& materials, const String& file_path);
    bool write_shapes_with_layers(const Array& shapes, const Array& layer_names, const String& file_path);

    // Validation and information
    bool validate_shape_for_step(const Ref<ocgd_shape>& shape);
    Dictionary get_write_statistics();
    Array get_supported_units();
    Array get_supported_protocols();
    Array get_supported_schemas();

    // Error handling
    String get_last_error() const;
    void clear_last_error();

    // Configuration presets
    void set_configuration_preset(const String& preset_name);
    Dictionary get_current_configuration();
    void load_configuration(const Dictionary& config);

    // File information
    Dictionary get_step_file_info(const String& file_path);
    bool is_valid_step_file(const String& file_path);

    // Quality control
    void set_quality_level(int level);
    int get_quality_level() const;

    void set_angular_tolerance(double tolerance);
    double get_angular_tolerance() const;

    void set_linear_tolerance(double tolerance);
    double get_linear_tolerance() const;

    // Advanced features
    void set_compress_output(bool enable);
    bool get_compress_output() const;

    void set_write_shape_names(bool enable);
    bool get_write_shape_names() const;

    void set_write_surface_colors(bool enable);
    bool get_write_surface_colors() const;

    void set_write_curve_colors(bool enable);
    bool get_write_curve_colors() const;
};

} // namespace godot

#endif // OCGD_STEP_WRITER_H