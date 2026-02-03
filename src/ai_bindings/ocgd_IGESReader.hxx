#ifndef _ocgd_IGESReader_HeaderFile
#define _ocgd_IGESReader_HeaderFile

/**
 * ocgd_IGESReader.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE IGES import functionality.
 *
 * This class provides methods to import IGES (Initial Graphics Exchange Specification) files,
 * a widely used CAD data exchange format. IGES files can contain 2D and 3D geometric data,
 * including curves, surfaces, solids, and assembly information.
 *
 * Features:
 * - Read IGES files (.igs, .iges formats)
 * - Support for both visible and all entities import
 * - Transfer individual entities or complete models
 * - Progress reporting and error checking
 * - Shape validation and healing options
 * - Statistics and diagnostic information
 *
 * The reader follows the standard OpenCASCADE workflow:
 * 1. Load IGES file into memory
 * 2. Check file validity and report errors
 * 3. Transfer entities to OpenCASCADE shapes
 * 4. Validate and fix resulting geometry
 *
 * Original OCCT header: <opencascade/IGESControl_Reader.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <opencascade/IGESControl_Reader.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/IGESData_IGESModel.hxx>
#include <opencascade/IFSelect_ReturnStatus.hxx>
#include <opencascade/IFSelect_PrintFail.hxx>
#include <opencascade/IFSelect_PrintCount.hxx>
#include <opencascade/Message_ProgressRange.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_IGESReader
 *
 * Wrapper class to expose OpenCASCADE IGES import functionality to Godot.
 *
 * This class provides comprehensive IGES file import capabilities including
 * file validation, entity selection, transfer options, and diagnostic reporting.
 * It supports both simple single-shape imports and complex multi-entity transfers
 * with detailed control over the import process.
 */
class ocgd_IGESReader : public RefCounted {
    GDCLASS(ocgd_IGESReader, RefCounted);

protected:
    static void _bind_methods();

private:
    IGESControl_Reader* _reader;
    bool _owns_reader;

public:
    //! Creates a reader from scratch
    ocgd_IGESReader();

    //! Destructor
    virtual ~ocgd_IGESReader();

    //! Clear the reader and reset to initial state
    void clear();

    //! Set whether to read only visible entities (true) or all entities (false)
    void set_read_visible(bool read_visible);

    //! Get the current visible-only reading setting
    bool get_read_visible() const;

    // File Loading Methods

    //! Load an IGES file into memory
    //! @param filename path to the IGES file
    //! @return status code (0 = success, >0 = error)
    int read_file(const String& filename);

    //! Load IGES data from a stream
    //! @param name auxiliary name for the data source
    //! @param data byte array containing IGES file data
    //! @return status code (0 = success, >0 = error)
    int read_stream(const String& name, const PackedByteArray& data);

    // Validation and Checking Methods

    //! Check the loaded IGES file for errors and warnings
    //! @param fails_only if true, report only failures; if false, report all issues
    //! @return Dictionary with validation results
    Dictionary check_file(bool fails_only = false);

    //! Print check results to console
    //! @param fails_only if true, print only failures
    //! @param mode print mode (0=short, 1=medium, 2=detailed)
    void print_check_load(bool fails_only = false, int mode = 1);

    // Transfer Methods

    //! Get the number of root entities available for transfer
    int nb_roots_for_transfer();

    //! Transfer all root entities from the loaded file
    //! @param only_visible if true, transfer only visible entities
    //! @return number of shapes successfully transferred
    int transfer_roots(bool only_visible = true);

    //! Transfer a specific entity by index
    //! @param entity_index 1-based index of entity to transfer
    //! @return true if transfer succeeded
    bool transfer_entity(int entity_index);

    //! Transfer multiple entities by indices
    //! @param entity_indices array of 1-based entity indices
    //! @return number of entities successfully transferred
    int transfer_list(const Array& entity_indices);

    // Result Access Methods

    //! Check if the last transfer operation was successful
    bool is_done() const;

    //! Get the number of shapes produced by transfer operations
    int nb_shapes() const;

    //! Get a specific shape by index
    //! @param shape_index 1-based index of shape to retrieve
    //! @return the requested shape, or null if index is invalid
    Ref<ocgd_TopoDS_Shape> get_shape(int shape_index);

    //! Get all transferred shapes
    //! @return array of all transferred shapes
    Array get_all_shapes();

    //! Get a single shape (useful when only one shape is expected)
    //! If multiple shapes exist, returns a compound containing all shapes
    Ref<ocgd_TopoDS_Shape> get_one_shape();

    // Convenience Methods

    //! Simple IGES import: load file and transfer all visible entities
    //! @param filename path to IGES file
    //! @return the imported shape(s) as a single shape or compound
    Ref<ocgd_TopoDS_Shape> import_file(const String& filename);

    //! Import IGES with validation
    //! @param filename path to IGES file
    //! @param check_validity if true, validate the file before transfer
    //! @return Dictionary with "shape" and "validation_results"
    Dictionary import_file_with_validation(const String& filename, bool check_validity = true);

    // Information and Statistics Methods

    //! Get information about the loaded IGES model
    //! @return Dictionary with model statistics and header information
    Dictionary get_model_info();

    //! Get statistics about entity types in the model
    //! @return Dictionary with counts of different entity types
    Dictionary get_entity_statistics();

    //! Print transfer statistics to console
    //! @param fail_warn level of warnings to print (0=none, 1=fails, 2=all)
    //! @param mode detail level (0=short, 1=medium, 2=detailed)
    void print_transfer_info(int fail_warn = 1, int mode = 1);

    // Advanced Options

    //! Set shape healing parameters for the transfer process
    //! @param fix_small_edges if true, fix small edges during transfer
    //! @param fix_lacking_edges if true, fix lacking edges
    //! @param fix_small_faces if true, fix small faces
    void set_shape_healing_options(bool fix_small_edges = true, 
                                  bool fix_lacking_edges = true, 
                                  bool fix_small_faces = true);

    //! Set precision parameters for the transfer
    //! @param precision geometric precision for transfer operations
    void set_precision(double precision);

    //! Get the current precision setting
    double get_precision() const;

    // Status and Error Reporting

    //! Get detailed error information from the last operation
    //! @return Dictionary with error details and suggestions
    Dictionary get_last_error_info();

    //! Check if the reader has a valid loaded model
    bool has_model() const;

    //! Get the IGES file format version information
    //! @return Dictionary with version and format details
    Dictionary get_format_info();

    // Batch Processing

    //! Import multiple IGES files
    //! @param filenames array of file paths to import
    //! @return array of imported shapes (may contain nulls for failed imports)
    Array import_multiple_files(const Array& filenames);

    //! Validate multiple IGES files without importing
    //! @param filenames array of file paths to validate
    //! @return array of validation result dictionaries
    Array validate_multiple_files(const Array& filenames);

    // Entity Information

    //! Get information about a specific entity
    //! @param entity_index 1-based entity index
    //! @return Dictionary with entity type, properties, and status
    Dictionary get_entity_info(int entity_index);

    //! Get list of all entity types present in the model
    //! @return Array of entity type names
    Array get_entity_types();

    //! Find entities of a specific type
    //! @param entity_type name or number of entity type to find
    //! @return Array of entity indices of the specified type
    Array find_entities_by_type(const String& entity_type);

    // Status enumeration for return codes
    enum StatusCode {
        STATUS_OK = IFSelect_RetDone,
        STATUS_WARNING = IFSelect_RetDone,
        STATUS_ERROR = IFSelect_RetError,
        STATUS_FAIL = IFSelect_RetFail,
        STATUS_VOID = IFSelect_RetVoid,
        STATUS_STOP = IFSelect_RetStop
    };

    // Print modes for diagnostic output
    enum PrintMode {
        PRINT_SHORT = IFSelect_CountByItem,
        PRINT_MEDIUM = IFSelect_CountSummary, 
        PRINT_DETAILED = IFSelect_CountByItem
    };

    // Internal access to the wrapped OpenCASCADE object
    IGESControl_Reader* get_occt_reader() const { return _reader; }

private:
    //! Internal helper to convert IFSelect_ReturnStatus to integer
    int status_to_int(IFSelect_ReturnStatus status) const;

    //! Internal helper to get model statistics
    Dictionary extract_model_statistics() const;

    //! Internal helper for entity information extraction
    Dictionary extract_entity_info(int entity_index) const;
};

VARIANT_ENUM_CAST(ocgd_IGESReader::StatusCode);
VARIANT_ENUM_CAST(ocgd_IGESReader::PrintMode);

#endif // _ocgd_IGESReader_HeaderFile