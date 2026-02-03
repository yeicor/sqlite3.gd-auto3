/**
 * ocgd_AdvancedMeshExporter.hxx
 *
 * Godot GDExtension wrapper header for OpenCASCADE advanced mesh export functionality.
 *
 * This class provides comprehensive mesh export capabilities with support for:
 * - Multiple file formats (PLY, OBJ, STL)
 * - Color and material information
 * - Custom mesh properties and attributes
 * - High-quality triangulation control
 * - Texture coordinate export
 * - Normal vector export
 * - Progress monitoring
 *
 * Features:
 * - Format-specific optimization
 * - Color per vertex/face support
 * - Material property preservation
 * - Custom coordinate system conversion
 * - Export validation and quality checks
 * - Batch export capabilities
 * - Memory-efficient processing
 *
 * Original OCCT headers: <opencascade/RWPly_CafWriter.hxx>,
 *                       <opencascade/RWObj_CafWriter.hxx>,
 *                       <opencascade/StlAPI_Writer.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#ifndef OCGD_ADVANCED_MESH_EXPORTER_HXX
#define OCGD_ADVANCED_MESH_EXPORTER_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/RWPly_CafWriter.hxx>
#include <opencascade/RWObj_CafWriter.hxx>
#include <opencascade/StlAPI_Writer.hxx>
#include <opencascade/TDocStd_Document.hxx>
#include <opencascade/XCAFDoc_DocumentTool.hxx>
#include <opencascade/XCAFDoc_ShapeTool.hxx>
#include <opencascade/XCAFDoc_ColorTool.hxx>
#include <opencascade/XCAFDoc_MaterialTool.hxx>
#include <opencascade/RWMesh_CoordinateSystemConverter.hxx>
#include <opencascade/Quantity_Color.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/BRepMesh_IncrementalMesh.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/gp_Vec3f.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * @brief Advanced mesh exporter for OpenCASCADE shapes with comprehensive format support
 * 
 * This class provides a unified interface for exporting OpenCASCADE shapes to various
 * mesh formats with advanced features including color information, material properties,
 * and high-quality triangulation control.
 */
class ocgd_AdvancedMeshExporter : public RefCounted {
    GDCLASS(ocgd_AdvancedMeshExporter, RefCounted)

public:
    /**
     * @brief Supported export formats
     */
    enum ExportFormat {
        FORMAT_PLY = 0,     ///< Stanford PLY format (supports colors, normals, textures)
        FORMAT_OBJ = 1,     ///< Wavefront OBJ format (supports materials, textures)
        FORMAT_STL_ASCII = 2,   ///< STL ASCII format (geometry only)
        FORMAT_STL_BINARY = 3   ///< STL Binary format (geometry only)
    };

    /**
     * @brief Color extraction modes
     */
    enum ColorMode {
        COLOR_NONE = 0,         ///< No color information
        COLOR_PER_SHAPE = 1,    ///< Single color per shape
        COLOR_PER_FACE = 2,     ///< Color per face
        COLOR_PER_VERTEX = 3    ///< Interpolated color per vertex
    };

    /**
     * @brief Coordinate system conversion options
     */
    enum CoordinateSystem {
        COORD_OCCT = 0,     ///< Keep OpenCASCADE coordinate system (Z-up, right-handed)
        COORD_BLENDER = 1,  ///< Convert to Blender (Z-up, right-handed)
        COORD_UNITY = 2,    ///< Convert to Unity (Y-up, left-handed)
        COORD_UNREAL = 3,   ///< Convert to Unreal (Z-up, left-handed)
        COORD_CUSTOM = 4    ///< Use custom transformation matrix
    };

private:
    Handle(TDocStd_Document) _document;
    Handle(XCAFDoc_ShapeTool) _shape_tool;
    Handle(XCAFDoc_ColorTool) _color_tool;
    Handle(XCAFDoc_MaterialTool) _material_tool;
    
    // Export settings
    ExportFormat _format;
    ColorMode _color_mode;
    CoordinateSystem _coordinate_system;
    Dictionary _custom_transform;
    
    // Mesh quality settings
    double _linear_deflection;
    double _angular_deflection;
    bool _relative_deflection;
    bool _parallel_processing;
    
    // Export options
    bool _export_normals;
    bool _export_uv_coordinates;
    bool _export_materials;
    bool _merge_vertices;
    double _vertex_tolerance;
    
    // Progress tracking
    mutable String _last_error;
    mutable int _progress_current;
    mutable int _progress_total;

protected:
    static void _bind_methods();

public:
    ocgd_AdvancedMeshExporter();
    virtual ~ocgd_AdvancedMeshExporter();

    // === Configuration Methods ===
    
    /**
     * @brief Set the export format
     */
    void set_export_format(ExportFormat format);
    ExportFormat get_export_format() const;

    /**
     * @brief Set color extraction mode
     */
    void set_color_mode(ColorMode mode);
    ColorMode get_color_mode() const;

    /**
     * @brief Set coordinate system conversion
     */
    void set_coordinate_system(CoordinateSystem system);
    CoordinateSystem get_coordinate_system() const;

    /**
     * @brief Set custom transformation matrix for coordinate conversion
     */
    void set_custom_transform(const Dictionary& transform);
    Dictionary get_custom_transform() const;

    // === Mesh Quality Settings ===

    /**
     * @brief Set linear deflection for triangulation
     */
    void set_linear_deflection(double deflection);
    double get_linear_deflection() const;

    /**
     * @brief Set angular deflection for triangulation
     */
    void set_angular_deflection(double deflection);
    double get_angular_deflection() const;

    /**
     * @brief Enable/disable relative deflection
     */
    void set_relative_deflection(bool enabled);
    bool get_relative_deflection() const;

    /**
     * @brief Enable/disable parallel processing
     */
    void set_parallel_processing(bool enabled);
    bool get_parallel_processing() const;

    // === Export Options ===

    /**
     * @brief Enable/disable normal vector export
     */
    void set_export_normals(bool enabled);
    bool get_export_normals() const;

    /**
     * @brief Enable/disable UV coordinate export
     */
    void set_export_uv_coordinates(bool enabled);
    bool get_export_uv_coordinates() const;

    /**
     * @brief Enable/disable material export
     */
    void set_export_materials(bool enabled);
    bool get_export_materials() const;

    /**
     * @brief Enable/disable vertex merging
     */
    void set_merge_vertices(bool enabled);
    bool get_merge_vertices() const;

    /**
     * @brief Set vertex tolerance for merging
     */
    void set_vertex_tolerance(double tolerance);
    double get_vertex_tolerance() const;

    // === Main Export Methods ===

    /**
     * @brief Export single shape to file
     */
    bool export_shape(const Ref<ocgd_TopoDS_Shape>& shape, const String& file_path);

    /**
     * @brief Export multiple shapes to file with individual colors
     */
    bool export_shapes(const Array& shapes, const Array& colors, const String& file_path);

    /**
     * @brief Export shape with manual color assignment
     */
    bool export_shape_with_colors(const Ref<ocgd_TopoDS_Shape>& shape, 
                                  const PackedColorArray& face_colors,
                                  const String& file_path);

    /**
     * @brief Extract mesh data to Godot arrays
     */
    Dictionary extract_mesh_data(const Ref<ocgd_TopoDS_Shape>& shape, bool compute_normals = true);

    /**
     * @brief Extract colored mesh data to Godot arrays
     */
    Dictionary extract_colored_mesh_data(const Ref<ocgd_TopoDS_Shape>& shape);

    // === Validation and Information ===

    /**
     * @brief Validate shape for export
     */
    Dictionary validate_shape(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Get estimated file size for export
     */
    int estimate_file_size(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Get supported file extensions for current format
     */
    PackedStringArray get_supported_extensions() const;

    /**
     * @brief Check if format supports specific features
     */
    Dictionary get_format_capabilities(ExportFormat format) const;

    // === Progress and Error Handling ===

    /**
     * @brief Get current export progress (0-100)
     */
    int get_progress() const;

    /**
     * @brief Get last error message
     */
    String get_last_error() const;

    /**
     * @brief Clear error state
     */
    void clear_error();

    // === Utility Methods ===

    /**
     * @brief Apply triangulation to shape with current settings
     */
    bool triangulate_shape(const Ref<ocgd_TopoDS_Shape>& shape, bool compute_normals = true) const;

    /**
     * @brief Validate triangulation data integrity
     */
    bool validate_triangulation(const Handle(Poly_Triangulation)& triangulation) const;

    /**
     * @brief Get mesh statistics for a shape
     */
    Dictionary get_mesh_statistics(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Optimize mesh data by removing duplicates and improving topology
     */
    Dictionary optimize_mesh_data(const Dictionary& mesh_data) const;

private:
    // === Internal Helper Methods ===
    
    /**
     * @brief Initialize XCAF document for color/material handling
     */
    bool initialize_document();

    /**
     * @brief Setup coordinate system converter
     */
    RWMesh_CoordinateSystemConverter create_coordinate_converter() const;

    /**
     * @brief Extract color information from shape
     */
    PackedColorArray extract_colors(const TopoDS_Shape& shape) const;

    /**
     * @brief Convert mesh data based on coordinate system settings
     */
    void transform_mesh_data(PackedVector3Array& vertices, 
                           PackedVector3Array& normals) const;

    /**
     * @brief Export to PLY format
     */
    bool export_ply(const TopoDS_Shape& shape, const String& file_path);

    /**
     * @brief Export to OBJ format
     */
    bool export_obj(const TopoDS_Shape& shape, const String& file_path);

    /**
     * @brief Export to STL format
     */
    bool export_stl(const TopoDS_Shape& shape, const String& file_path);

    /**
     * @brief Update progress tracking
     */
    void update_progress(int current, int total) const;

    /**
     * @brief Set error message
     */
    void set_error(const String& error) const;
};

VARIANT_ENUM_CAST(ocgd_AdvancedMeshExporter::ExportFormat);
VARIANT_ENUM_CAST(ocgd_AdvancedMeshExporter::ColorMode);
VARIANT_ENUM_CAST(ocgd_AdvancedMeshExporter::CoordinateSystem);

#endif // OCGD_ADVANCED_MESH_EXPORTER_HXX