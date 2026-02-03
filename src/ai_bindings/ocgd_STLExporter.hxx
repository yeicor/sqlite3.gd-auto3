#ifndef _ocgd_STLExporter_HeaderFile
#define _ocgd_STLExporter_HeaderFile

/**
 * ocgd_STLExporter.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE STL export functionality.
 *
 * This class provides methods to export OpenCASCADE shapes to STL format,
 * supporting both ASCII and binary STL file formats. STL (STereoLithography)
 * is a widely used file format for 3D printing and rapid prototyping.
 *
 * Features:
 * - Export shapes to ASCII or binary STL files
 * - Control over mesh quality and deflection parameters
 * - Progress reporting for large exports
 * - Validation of shapes before export
 * - Support for compound shapes and assemblies
 *
 * Original OCCT header: <opencascade/StlAPI_Writer.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include <opencascade/StlAPI_Writer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/Message_ProgressRange.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_STLExporter
 *
 * Wrapper class to expose OpenCASCADE STL export functionality to Godot.
 *
 * This class provides methods to export OpenCASCADE shapes to STL format,
 * supporting both ASCII and binary STL file formats. The class handles
 * shape validation, mesh quality control, and provides progress feedback
 * for large export operations.
 */
class ocgd_STLExporter : public RefCounted {
    GDCLASS(ocgd_STLExporter, RefCounted);

protected:
    static void _bind_methods();

private:
    StlAPI_Writer* _writer;
    bool _owns_writer;

public:
    //! Creates a writer object with default parameters (ASCII mode enabled)
    ocgd_STLExporter();

    //! Destructor
    virtual ~ocgd_STLExporter();

    //! Set the output format mode
    //! @param ascii_mode if true, generates ASCII STL file; if false, generates binary STL file
    void set_ascii_mode(bool ascii_mode);

    //! Get the current output format mode
    //! @return true if ASCII mode is enabled, false for binary mode
    bool get_ascii_mode() const;

    //! Export a shape to STL file
    //! @param shape the shape to export
    //! @param filename the output file path
    //! @return true if export succeeded, false otherwise
    bool write_file(const Ref<ocgd_TopoDS_Shape>& shape, const String& filename);

    //! Export a shape to STL file with progress reporting
    //! @param shape the shape to export
    //! @param filename the output file path
    //! @param progress_callback name of method to call for progress updates (0.0 to 1.0)
    //! @return true if export succeeded, false otherwise
    bool write_file_with_progress(const Ref<ocgd_TopoDS_Shape>& shape, 
                                 const String& filename,
                                 const String& progress_callback = "");

    //! Check if a shape can be exported to STL
    //! @param shape the shape to validate
    //! @return true if the shape is valid for STL export
    bool can_export_shape(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Get estimated file size for STL export
    //! @param shape the shape to analyze
    //! @param ascii_mode whether to estimate for ASCII or binary format
    //! @return estimated file size in bytes
    int get_estimated_file_size(const Ref<ocgd_TopoDS_Shape>& shape, bool ascii_mode) const;

    //! Get triangle count for a shape (requires triangulation)
    //! @param shape the shape to analyze
    //! @return number of triangles that would be exported
    int get_triangle_count(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Validate shape for STL export and return error message if any
    //! @param shape the shape to validate
    //! @return empty string if valid, error message otherwise
    String validate_shape_for_export(const Ref<ocgd_TopoDS_Shape>& shape) const;

    //! Export shape to STL format and return as byte array
    //! @param shape the shape to export
    //! @param ascii_mode whether to use ASCII format
    //! @return STL data as PackedByteArray, empty if failed
    PackedByteArray export_to_bytes(const Ref<ocgd_TopoDS_Shape>& shape, bool ascii_mode) const;

    //! Quick export with default settings
    //! @param shape the shape to export
    //! @param filename the output file path
    //! @param ascii_mode whether to use ASCII format (default: true)
    //! @return true if export succeeded
    bool quick_export(const Ref<ocgd_TopoDS_Shape>& shape, const String& filename, bool ascii_mode = true);

    // Static utility methods for common export scenarios

    //! Export multiple shapes to separate STL files
    //! @param shapes array of shapes to export
    //! @param base_filename base filename (will append index numbers)
    //! @param ascii_mode whether to use ASCII format
    //! @return number of successfully exported files
    int export_multiple_shapes(const Array& shapes, const String& base_filename, bool ascii_mode = true);

    //! Get recommended deflection for STL export based on shape size
    //! @param shape the shape to analyze
    //! @param quality_factor quality factor (0.1 = coarse, 1.0 = fine)
    //! @return recommended linear deflection value
    double get_recommended_deflection(const Ref<ocgd_TopoDS_Shape>& shape, double quality_factor = 0.5) const;

    // Internal access to the wrapped OpenCASCADE object
    StlAPI_Writer* get_occt_writer() const { return _writer; }
};

#endif // _ocgd_STLExporter_HeaderFile