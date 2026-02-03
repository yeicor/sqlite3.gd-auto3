#ifndef OCGD_IGES_WRITER_H
#define OCGD_IGES_WRITER_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include "ocgd_shape.h"

namespace godot {

class ocgd_iges_writer : public RefCounted {
    GDCLASS(ocgd_iges_writer, RefCounted)

private:
    String last_error;
    String system_identifier;
    String preprocessor_version;
    String author_name;
    String organization;
    String description;
    String model_space_units;
    double global_scale;
    bool write_colors;
    bool write_layers;
    bool write_names;
    bool validate_on_write;
    bool write_precision_mode;
    double precision_value;
    bool merge_surfaces;
    int iges_version;
    String drafting_standard;
    String model_units;
    double unit_scale_factor;
    bool write_brep_entities;
    bool write_trimmed_surfaces;
    bool write_analytical_surfaces;
    bool optimize_curves;
    double curve_tolerance;
    double surface_tolerance;
    bool write_isoparametric_curves;

protected:
    static void _bind_methods();

public:
    ocgd_iges_writer();
    ~ocgd_iges_writer();

    // Factory method
    static Ref<ocgd_iges_writer> new_iges_writer();

    // Core writing functionality
    bool write_shape(const Ref<ocgd_shape>& shape, const String& file_path);
    bool write_shapes(const Array& shapes, const String& file_path);

    // Configuration methods
    void set_system_identifier(const String& identifier);
    String get_system_identifier() const;

    void set_preprocessor_version(const String& version);
    String get_preprocessor_version() const;

    void set_author_name(const String& name);
    String get_author_name() const;

    void set_organization(const String& org);
    String get_organization() const;

    void set_description(const String& desc);
    String get_description() const;

    void set_model_space_units(const String& units);
    String get_model_space_units() const;

    void set_global_scale(double scale);
    double get_global_scale() const;

    // Feature control
    void set_write_colors(bool enable);
    bool get_write_colors() const;

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

    // IGES version and standards
    void set_iges_version(int version);
    int get_iges_version() const;

    void set_drafting_standard(const String& standard);
    String get_drafting_standard() const;

    void set_model_units(const String& units);
    String get_model_units() const;

    void set_unit_scale_factor(double factor);
    double get_unit_scale_factor() const;

    // Entity type control
    void set_write_brep_entities(bool enable);
    bool get_write_brep_entities() const;

    void set_write_trimmed_surfaces(bool enable);
    bool get_write_trimmed_surfaces() const;

    void set_write_analytical_surfaces(bool enable);
    bool get_write_analytical_surfaces() const;

    void set_optimize_curves(bool enable);
    bool get_optimize_curves() const;

    // Tolerance settings
    void set_curve_tolerance(double tolerance);
    double get_curve_tolerance() const;

    void set_surface_tolerance(double tolerance);
    double get_surface_tolerance() const;

    // Advanced writing methods
    bool write_with_metadata(const Ref<ocgd_shape>& shape, const String& file_path, const Dictionary& metadata);
    bool write_shapes_with_colors(const Array& shapes, const Array& colors, const String& file_path);
    bool write_shapes_with_layers(const Array& shapes, const Array& layer_names, const String& file_path);

    // Validation and information
    bool validate_shape_for_iges(const Ref<ocgd_shape>& shape);
    Dictionary get_write_statistics();
    Array get_supported_units();
    Array get_supported_versions();

    // Error handling
    String get_last_error() const;
    void clear_last_error();

    // Configuration presets
    void set_configuration_preset(const String& preset_name);
    Dictionary get_current_configuration();
    void load_configuration(const Dictionary& config);

    // File information
    Dictionary get_iges_file_info(const String& file_path);
    bool is_valid_iges_file(const String& file_path);

    // Quality control
    void set_quality_level(int level);
    int get_quality_level() const;

    void set_angular_tolerance(double tolerance);
    double get_angular_tolerance() const;

    void set_linear_tolerance(double tolerance);
    double get_linear_tolerance() const;

    // Advanced features
    void set_write_parameter_space_curves(bool enable);
    bool get_write_parameter_space_curves() const;

    void set_write_isoparametric_curves(bool enable);
    bool get_write_isoparametric_curves() const;

    void set_convert_surfaces_to_bsplines(bool enable);
    bool get_convert_surfaces_to_bsplines() const;

    void set_convert_curves_to_bsplines(bool enable);
    bool get_convert_curves_to_bsplines() const;
};

} // namespace godot

#endif // OCGD_IGES_WRITER_H