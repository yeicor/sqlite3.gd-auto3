/**
 * ocgd_CADFileImporter.hxx
 *
 * Godot GDExtension wrapper header for OpenCASCADE comprehensive CAD file import functionality.
 *
 * This class provides advanced CAD file import capabilities with support for:
 * - Multiple CAD formats (STEP, IGES, BREP, OBJ, PLY, STL)
 * - Color and material information extraction
 * - Metadata and properties reading
 * - Assembly structure preservation
 * - Unit conversion and scaling
 * - Progress monitoring and error reporting
 * - Batch import capabilities
 * - Quality validation and repair
 *
 * Features:
 * - Format auto-detection and validation
 * - XCAF (Extended CAF) support for colors and materials
 * - Layer and group information extraction
 * - Custom properties and attributes reading
 * - Assembly hierarchy with positioning
 * - Geometric validation and repair options
 * - Memory-efficient streaming for large files
 * - Multi-threaded processing support
 *
 * Original OCCT headers: <opencascade/STEPCAFControl_Reader.hxx>,
 *                       <opencascade/IGESCAFControl_Reader.hxx>,
 *                       <opencascade/XCAFDoc_DocumentTool.hxx>,
 *                       <opencascade/RWObj_CafReader.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#ifndef OCGD_CAD_FILE_IMPORTER_HXX
#define OCGD_CAD_FILE_IMPORTER_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/packed_color_array.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/STEPCAFControl_Reader.hxx>
#include <opencascade/IGESCAFControl_Reader.hxx>
#include <opencascade/BRep_Builder.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/TDocStd_Document.hxx>
#include <opencascade/XCAFApp_Application.hxx>
#include <opencascade/XCAFDoc_DocumentTool.hxx>
#include <opencascade/XCAFDoc_ShapeTool.hxx>
#include <opencascade/XCAFDoc_ColorTool.hxx>
#include <opencascade/XCAFDoc_MaterialTool.hxx>
#include <opencascade/XCAFDoc_LayerTool.hxx>
#include <opencascade/TDF_Label.hxx>
#include <opencascade/TDF_LabelSequence.hxx>
#include <opencascade/TDataStd_Name.hxx>
#include <opencascade/TDataStd_TreeNode.hxx>
#include <opencascade/Quantity_Color.hxx>
#include <opencascade/Quantity_ColorRGBA.hxx>
#include <opencascade/StlAPI_Reader.hxx>
#include <opencascade/RWObj_CafReader.hxx>

#include <opencascade/Interface_Static.hxx>
#include <opencascade/Units_Dimensions.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/ShapeFix_Shape.hxx>
#include <opencascade/Message_ProgressRange.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * @brief Comprehensive CAD file importer with advanced metadata support
 * 
 * This class provides a unified interface for importing various CAD file formats
 * with comprehensive support for colors, materials, assembly structures, and
 * metadata extraction.
 */
class ocgd_CADFileImporter : public RefCounted {
    GDCLASS(ocgd_CADFileImporter, RefCounted)

public:
    /**
     * @brief Supported import formats
     */
    enum ImportFormat {
        FORMAT_AUTO = 0,        ///< Auto-detect format from file extension
        FORMAT_STEP = 1,        ///< STEP format (.step, .stp)
        FORMAT_IGES = 2,        ///< IGES format (.iges, .igs)
        FORMAT_BREP = 3,        ///< OpenCASCADE BREP format (.brep)
        FORMAT_STL = 4,         ///< STL format (.stl)
        FORMAT_OBJ = 5,         ///< Wavefront OBJ format (.obj)
        FORMAT_PLY = 6          ///< Stanford PLY format (.ply)
    };

    /**
     * @brief Import modes for handling multiple shapes
     */
    enum ImportMode {
        MODE_SINGLE_SHAPE = 0,  ///< Merge all shapes into single compound
        MODE_ASSEMBLY = 1,      ///< Preserve assembly structure
        MODE_SEPARATE_SHAPES = 2 ///< Import as separate shapes
    };

    /**
     * @brief Unit conversion options
     */
    enum UnitType {
        UNIT_MILLIMETER = 0,
        UNIT_CENTIMETER = 1,
        UNIT_METER = 2,
        UNIT_INCH = 3,
        UNIT_FOOT = 4,
        UNIT_AUTO = 5           ///< Use file's native units
    };

    /**
     * @brief Quality repair levels
     */
    enum RepairLevel {
        REPAIR_NONE = 0,        ///< No repair
        REPAIR_BASIC = 1,       ///< Basic topology fixes
        REPAIR_ADVANCED = 2,    ///< Advanced geometry repair
        REPAIR_COMPLETE = 3     ///< Complete rebuild if necessary
    };

private:
    // Document for XCAF operations
    Handle(TDocStd_Document) _document;
    Handle(XCAFDoc_ShapeTool) _shape_tool;
    Handle(XCAFDoc_ColorTool) _color_tool;
    Handle(XCAFDoc_MaterialTool) _material_tool;
    Handle(XCAFDoc_LayerTool) _layer_tool;
    
    // Import settings
    ImportFormat _format;
    ImportMode _import_mode;
    UnitType _target_units;
    RepairLevel _repair_level;
    
    // Processing options
    bool _read_colors;
    bool _read_materials;
    bool _read_layers;
    bool _read_metadata;
    bool _read_assembly_structure;
    bool _validate_geometry;
    bool _fix_geometry;
    double _scaling_factor;
    double _tolerance;
    
    // Progress and error tracking
    mutable String _last_error;
    mutable String _last_warning;
    mutable int _progress_current;
    mutable int _progress_total;
    mutable bool _operation_cancelled;
    
    // Import results cache
    mutable Dictionary _last_import_info;
    mutable Array _imported_shapes;
    mutable Dictionary _assembly_structure;
    mutable Dictionary _color_information;
    mutable Dictionary _material_information;

protected:
    static void _bind_methods();

public:
    ocgd_CADFileImporter();
    virtual ~ocgd_CADFileImporter();

    // === Configuration Methods ===
    
    /**
     * @brief Set import format (auto-detection if not specified)
     */
    void set_import_format(ImportFormat format);
    ImportFormat get_import_format() const;

    /**
     * @brief Set import mode for handling multiple shapes
     */
    void set_import_mode(ImportMode mode);
    ImportMode get_import_mode() const;

    /**
     * @brief Set target units for conversion
     */
    void set_target_units(UnitType units);
    UnitType get_target_units() const;

    /**
     * @brief Set geometry repair level
     */
    void set_repair_level(RepairLevel level);
    RepairLevel get_repair_level() const;

    /**
     * @brief Set custom scaling factor
     */
    void set_scaling_factor(double factor);
    double get_scaling_factor() const;

    /**
     * @brief Set geometric tolerance
     */
    void set_tolerance(double tolerance);
    double get_tolerance() const;

    // === Processing Options ===

    /**
     * @brief Enable/disable color information reading
     */
    void set_read_colors(bool enabled);
    bool get_read_colors() const;

    /**
     * @brief Enable/disable material information reading
     */
    void set_read_materials(bool enabled);
    bool get_read_materials() const;

    /**
     * @brief Enable/disable layer information reading
     */
    void set_read_layers(bool enabled);
    bool get_read_layers() const;

    /**
     * @brief Enable/disable metadata reading
     */
    void set_read_metadata(bool enabled);
    bool get_read_metadata() const;

    /**
     * @brief Enable/disable assembly structure preservation
     */
    void set_read_assembly_structure(bool enabled);
    bool get_read_assembly_structure() const;

    /**
     * @brief Enable/disable geometry validation
     */
    void set_validate_geometry(bool enabled);
    bool get_validate_geometry() const;

    /**
     * @brief Enable/disable automatic geometry fixing
     */
    void set_fix_geometry(bool enabled);
    bool get_fix_geometry() const;

    // === Main Import Methods ===

    /**
     * @brief Import CAD file and return primary shape
     */
    Ref<ocgd_TopoDS_Shape> import_file(const String& file_path);

    /**
     * @brief Import CAD file and return all shapes
     */
    Array import_file_multiple(const String& file_path);

    /**
     * @brief Import CAD file with full metadata extraction
     */
    Dictionary import_file_with_metadata(const String& file_path);

    /**
     * @brief Import multiple files in batch
     */
    Array import_files_batch(const PackedStringArray& file_paths);

    // === File Information ===

    /**
     * @brief Get file format information without importing
     */
    Dictionary analyze_file(const String& file_path);

    /**
     * @brief Validate file before import
     */
    Dictionary validate_file(const String& file_path);

    /**
     * @brief Get supported file extensions for format
     */
    PackedStringArray get_supported_extensions(ImportFormat format) const;

    /**
     * @brief Detect format from file extension or content
     */
    ImportFormat detect_format(const String& file_path) const;

    // === Assembly and Structure ===

    /**
     * @brief Get assembly structure from last import
     */
    Dictionary get_assembly_structure() const;

    /**
     * @brief Get shape hierarchy with names and labels
     */
    Dictionary get_shape_hierarchy() const;

    /**
     * @brief Get shape by name or label
     */
    Ref<ocgd_TopoDS_Shape> get_shape_by_name(const String& name) const;

    /**
     * @brief Get all shape names and labels
     */
    PackedStringArray get_shape_names() const;

    // === Color and Material Information ===

    /**
     * @brief Get color information for all shapes
     */
    Dictionary get_color_information() const;

    /**
     * @brief Get material information for all shapes
     */
    Dictionary get_material_information() const;

    /**
     * @brief Get layer information
     */
    Dictionary get_layer_information() const;

    /**
     * @brief Get color for specific shape
     */
    Color get_shape_color(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Get colors for all faces of a shape
     */
    PackedColorArray get_face_colors(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Get material properties for shape
     */
    Dictionary get_shape_material(const Ref<ocgd_TopoDS_Shape>& shape) const;

    // === Metadata and Properties ===

    /**
     * @brief Get file metadata (author, creation date, units, etc.)
     */
    Dictionary get_file_metadata() const;

    /**
     * @brief Get custom properties for shape
     */
    Dictionary get_shape_properties(const Ref<ocgd_TopoDS_Shape>& shape) const;

    /**
     * @brief Get all custom properties from file
     */
    Dictionary get_all_properties() const;

    /**
     * @brief Get units information from file
     */
    Dictionary get_units_information() const;

    // === Quality and Validation ===

    /**
     * @brief Get import quality report
     */
    Dictionary get_import_quality_report() const;

    /**
     * @brief Get geometry validation results
     */
    Dictionary get_validation_results() const;

    /**
     * @brief Get repair operations performed
     */
    Array get_repair_operations() const;

    // === Progress and Error Handling ===

    /**
     * @brief Get import progress (0-100)
     */
    int get_progress() const;

    /**
     * @brief Cancel ongoing import operation
     */
    void cancel_import();

    /**
     * @brief Check if operation was cancelled
     */
    bool is_cancelled() const;

    /**
     * @brief Get last error message
     */
    String get_last_error() const;

    /**
     * @brief Get last warning message
     */
    String get_last_warning() const;

    /**
     * @brief Clear error and warning states
     */
    void clear_messages();

    // === Utility Methods ===

    /**
     * @brief Convert units between different systems
     */
    double convert_units(double value, UnitType from_units, UnitType to_units) const;

    /**
     * @brief Get import statistics from last operation
     */
    Dictionary get_import_statistics() const;

    /**
     * @brief Reset importer to default settings
     */
    void reset_settings();

private:
    // === Internal Helper Methods ===
    
    /**
     * @brief Initialize XCAF document and tools
     */
    bool initialize_document();

    /**
     * @brief Auto-detect file format
     */
    ImportFormat auto_detect_format(const String& file_path) const;

    /**
     * @brief Import STEP file with XCAF
     */
    bool import_step_file(const String& file_path);

    /**
     * @brief Import IGES file with XCAF
     */
    bool import_iges_file(const String& file_path);

    /**
     * @brief Import BREP file
     */
    bool import_brep_file(const String& file_path);

    /**
     * @brief Import STL file
     */
    bool import_stl_file(const String& file_path);

    /**
     * @brief Import OBJ file
     */
    bool import_obj_file(const String& file_path);

    /**
     * @brief Import PLY file
     */
    bool import_ply_file(const String& file_path);

    /**
     * @brief Process XCAF document after import
     */
    bool process_xcaf_document();

    /**
     * @brief Extract colors from XCAF document
     */
    void extract_colors();

    /**
     * @brief Extract materials from XCAF document
     */
    void extract_materials();

    /**
     * @brief Extract layers from XCAF document
     */
    void extract_layers();

    /**
     * @brief Extract assembly structure
     */
    void extract_assembly_structure();

    /**
     * @brief Extract metadata and properties
     */
    void extract_metadata();

    /**
     * @brief Apply scaling and unit conversion
     */
    void apply_scaling_and_units();

    /**
     * @brief Validate and repair geometry
     */
    bool validate_and_repair_geometry();

    /**
     * @brief Convert OCCT color to Godot Color
     */
    Color quantity_color_to_godot(const Quantity_Color& occt_color) const;

    /**
     * @brief Convert OCCT color RGBA to Godot Color
     */
    Color quantity_color_rgba_to_godot(const Quantity_ColorRGBA& occt_color) const;

    /**
     * @brief Get file extension from path
     */
    String get_file_extension(const String& file_path) const;

    /**
     * @brief Update progress tracking
     */
    void update_progress(int current, int total) const;

    /**
     * @brief Set error message
     */
    void set_error(const String& error) const;

    /**
     * @brief Set warning message
     */
    void set_warning(const String& warning) const;

    /**
     * @brief Check if operation should be cancelled
     */
    bool should_cancel() const;

    /**
     * @brief Clean up resources
     */
    void cleanup();
};

VARIANT_ENUM_CAST(ocgd_CADFileImporter::ImportFormat);
VARIANT_ENUM_CAST(ocgd_CADFileImporter::ImportMode);
VARIANT_ENUM_CAST(ocgd_CADFileImporter::UnitType);
VARIANT_ENUM_CAST(ocgd_CADFileImporter::RepairLevel);

#endif // OCGD_CAD_FILE_IMPORTER_HXX