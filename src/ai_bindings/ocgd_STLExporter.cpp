/**
 * ocgd_STLExporter.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE STL export functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_STLExporter.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/StlAPI_Writer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/BRep_Tool.hxx>
#include <opencascade/Poly_Triangulation.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/BRepBndLib.hxx>
#include <opencascade/Bnd_Box.hxx>
#include <opencascade/BRepCheck_Analyzer.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/Standard_Failure.hxx>

#include <fstream>

using namespace godot;

ocgd_STLExporter::ocgd_STLExporter() {
    try {
        _writer = new StlAPI_Writer();
        _owns_writer = true;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Failed to create writer - " + String(e.GetMessageString()));
        _writer = nullptr;
        _owns_writer = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Failed to create writer - " + String(e.what()));
        _writer = nullptr;
        _owns_writer = false;
    }
}

ocgd_STLExporter::~ocgd_STLExporter() {
    try {
        if (_owns_writer && _writer != nullptr) {
            delete _writer;
            _writer = nullptr;
        }
    } catch (...) {
        // Destructor should not throw, just log the issue
        UtilityFunctions::printerr("STLExporter: Warning - exception during destruction");
    }
}

void ocgd_STLExporter::set_ascii_mode(bool ascii_mode) {
    try {
        if (_writer == nullptr) {
            UtilityFunctions::printerr("STLExporter: Cannot set ASCII mode - writer is not initialized");
            return;
        }
        _writer->ASCIIMode() = ascii_mode ? Standard_True : Standard_False;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error setting ASCII mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error setting ASCII mode - " + String(e.what()));
    }
}

bool ocgd_STLExporter::get_ascii_mode() const {
    try {
        if (_writer == nullptr) {
            UtilityFunctions::printerr("STLExporter: Cannot get ASCII mode - writer is not initialized");
            return true; // Default to ASCII mode
        }
        return _writer->ASCIIMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error getting ASCII mode - " + String(e.GetMessageString()));
        return true;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error getting ASCII mode - " + String(e.what()));
        return true;
    }
}

bool ocgd_STLExporter::write_file(const Ref<ocgd_TopoDS_Shape>& shape, const String& filename) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("STLExporter: Cannot export - shape reference is null");
            return false;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("STLExporter: Cannot export - shape is null");
            return false;
        }

        if (_writer == nullptr) {
            UtilityFunctions::printerr("STLExporter: Cannot export - writer is not initialized");
            return false;
        }

        if (filename.is_empty()) {
            UtilityFunctions::printerr("STLExporter: Cannot export - filename is empty");
            return false;
        }

        std::string std_filename = filename.utf8().get_data();
        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("STLExporter: Cannot export - OpenCASCADE shape is null");
            return false;
        }

        Message_ProgressRange progress;
        Standard_Boolean result = _writer->Write(occt_shape, std_filename.c_str(), progress);

        if (result != Standard_True) {
            UtilityFunctions::printerr("STLExporter: Write operation failed for file: " + filename);
        }

        return result == Standard_True;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Exception writing file '" + filename + "' - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Exception writing file '" + filename + "' - " + String(e.what()));
        return false;
    }
}

bool ocgd_STLExporter::write_file_with_progress(const Ref<ocgd_TopoDS_Shape>& shape,
                                               const String& filename,
                                               const String& progress_callback) {
    // For now, just use the regular write method
    // Progress callback implementation would require more complex integration
    return write_file(shape, filename);
}

bool ocgd_STLExporter::can_export_shape(const Ref<ocgd_TopoDS_Shape>& shape) const {
    try {
        if (shape.is_null()) {
            return false;
        }

        if (shape->is_null()) {
            return false;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            return false;
        }

        // Check if shape has any faces that can be triangulated
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        if (!face_explorer.More()) {
            return false; // No faces to export
        }

        // Check if shape is valid
        BRepCheck_Analyzer analyzer(occt_shape);
        return analyzer.IsValid() == Standard_True;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error checking if shape can be exported - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error checking if shape can be exported - " + String(e.what()));
        return false;
    }
}

int ocgd_STLExporter::get_estimated_file_size(const Ref<ocgd_TopoDS_Shape>& shape, bool ascii_mode) const {
    try {
        if (shape.is_null() || shape->is_null()) {
            return 0;
        }

        int triangle_count = get_triangle_count(shape);
        if (triangle_count == 0) {
            return 0;
        }

        if (ascii_mode) {
            // ASCII STL format: ~200 bytes per triangle (including normal and vertices)
            return triangle_count * 200 + 100; // 100 bytes for header
        } else {
            // Binary STL format: 50 bytes per triangle + 80 byte header + 4 byte count
            return triangle_count * 50 + 84;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error estimating file size - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error estimating file size - " + String(e.what()));
        return 0;
    }
}

int ocgd_STLExporter::get_triangle_count(const Ref<ocgd_TopoDS_Shape>& shape) const {
    try {
        if (shape.is_null() || shape->is_null()) {
            return 0;
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();
        
        if (occt_shape.IsNull()) {
            return 0;
        }

        int total_triangles = 0;

        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (!triangulation.IsNull()) {
                total_triangles += triangulation->NbTriangles();
            }

            face_explorer.Next();
        }

        return total_triangles;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error counting triangles - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error counting triangles - " + String(e.what()));
        return 0;
    }
}

String ocgd_STLExporter::validate_shape_for_export(const Ref<ocgd_TopoDS_Shape>& shape) const {
    try {
        if (shape.is_null()) {
            return "Shape reference is null";
        }

        if (shape->is_null()) {
            return "Shape is null";
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            return "OpenCASCADE shape is null";
        }

        // Check if shape has faces
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        if (!face_explorer.More()) {
            return "Shape has no faces to export";
        }

        // Check if shape is valid
        BRepCheck_Analyzer analyzer(occt_shape);
        if (analyzer.IsValid() != Standard_True) {
            return "Shape has geometric errors";
        }

        // Check if any faces have triangulation
        bool has_triangulation = false;
        face_explorer.ReInit();
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());

            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (!triangulation.IsNull()) {
                has_triangulation = true;
                break;
            }

            face_explorer.Next();
        }

        if (!has_triangulation) {
            return "Shape has no triangulation data (run mesh generation first)";
        }

        return ""; // Valid for export

    } catch (const Standard_Failure& e) {
        return "Validation failed: " + String(e.GetMessageString());
    } catch (const std::exception& e) {
        return "Validation failed: " + String(e.what());
    }
}

PackedByteArray ocgd_STLExporter::export_to_bytes(const Ref<ocgd_TopoDS_Shape>& shape, bool ascii_mode) const {
    PackedByteArray result;

    try {
        if (shape.is_null() || shape->is_null()) {
            UtilityFunctions::printerr("STLExporter: Cannot export null shape to bytes");
            return result;
        }

        // Create a temporary filename
        String temp_filename = "temp_export.stl";

        // Set ASCII mode
        bool original_mode = get_ascii_mode();
        const_cast<ocgd_STLExporter*>(this)->set_ascii_mode(ascii_mode);

        // Export to file
        bool success = const_cast<ocgd_STLExporter*>(this)->write_file(shape, temp_filename);

        if (success) {
            // Read file back into byte array
            std::ifstream file(temp_filename.utf8().get_data(), std::ios::binary);
            if (file) {
                file.seekg(0, std::ios::end);
                size_t size = file.tellg();
                file.seekg(0, std::ios::beg);

                if (size > 0) {
                    result.resize(size);
                    file.read(reinterpret_cast<char*>(result.ptrw()), size);
                }
                file.close();

                // Clean up temp file
                std::remove(temp_filename.utf8().get_data());
            } else {
                UtilityFunctions::printerr("STLExporter: Failed to read back temporary file");
            }
        }

        // Restore original mode
        const_cast<ocgd_STLExporter*>(this)->set_ascii_mode(original_mode);

        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Exception exporting to bytes - " + String(e.GetMessageString()));
        return PackedByteArray();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Exception exporting to bytes - " + String(e.what()));
        return PackedByteArray();
    }
}

bool ocgd_STLExporter::quick_export(const Ref<ocgd_TopoDS_Shape>& shape, const String& filename, bool ascii_mode) {
    try {
        set_ascii_mode(ascii_mode);
        return write_file(shape, filename);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Exception in quick export - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Exception in quick export - " + String(e.what()));
        return false;
    }
}

int ocgd_STLExporter::export_multiple_shapes(const Array& shapes, const String& base_filename, bool ascii_mode) {
    int success_count = 0;

    try {
        if (base_filename.is_empty()) {
            UtilityFunctions::printerr("STLExporter: Cannot export multiple shapes - base filename is empty");
            return 0;
        }

        set_ascii_mode(ascii_mode);

        for (int i = 0; i < shapes.size(); i++) {
            try {
                Ref<ocgd_TopoDS_Shape> shape = shapes[i];
                if (shape.is_null()) {
                    UtilityFunctions::printerr("STLExporter: Skipping null shape at index " + String::num(i));
                    continue;
                }

                // Generate filename with index
                String filename = base_filename;
                int dot_pos = filename.rfind(".");
                if (dot_pos != -1) {
                    String base = filename.substr(0, dot_pos);
                    String ext = filename.substr(dot_pos);
                    filename = base + "_" + String::num(i) + ext;
                } else {
                    filename = filename + "_" + String::num(i) + ".stl";
                }

                if (write_file(shape, filename)) {
                    success_count++;
                } else {
                    UtilityFunctions::printerr("STLExporter: Failed to export shape " + String::num(i) + " to " + filename);
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("STLExporter: Exception exporting shape " + String::num(i) + " - " + String(e.GetMessageString()));
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("STLExporter: Exception exporting shape " + String::num(i) + " - " + String(e.what()));
            }
        }

        return success_count;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Exception in multiple export - " + String(e.GetMessageString()));
        return success_count;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Exception in multiple export - " + String(e.what()));
        return success_count;
    }
}

double ocgd_STLExporter::get_recommended_deflection(const Ref<ocgd_TopoDS_Shape>& shape, double quality_factor) const {
    try {
        if (shape.is_null() || shape->is_null()) {
            return 0.1; // Default deflection
        }

        const TopoDS_Shape& occt_shape = shape->get_occt_shape();

        if (occt_shape.IsNull()) {
            return 0.1;
        }

        // Validate quality factor
        if (!std::isfinite(quality_factor) || quality_factor < 0.0 || quality_factor > 1.0) {
            UtilityFunctions::printerr("STLExporter: Invalid quality factor, using 0.5. Value should be between 0.0 and 1.0, got: " + String::num(quality_factor));
            quality_factor = 0.5;
        }

        // Get bounding box
        Bnd_Box bounding_box;
        BRepBndLib::Add(occt_shape, bounding_box);

        if (bounding_box.IsVoid()) {
            return 0.1;
        }

        Standard_Real xmin, ymin, zmin, xmax, ymax, zmax;
        bounding_box.Get(xmin, ymin, zmin, xmax, ymax, zmax);

        // Calculate diagonal length
        double dx = xmax - xmin;
        double dy = ymax - ymin;
        double dz = zmax - zmin;
        double diagonal = sqrt(dx*dx + dy*dy + dz*dz);

        if (diagonal <= 0.0) {
            return 0.1;
        }

        // Recommend deflection as a fraction of the diagonal
        // quality_factor: 0.1 = coarse (1/100), 1.0 = fine (1/1000)
        double base_fraction = 0.01; // 1/100 for coarse
        double fine_fraction = 0.001; // 1/1000 for fine

        double fraction = base_fraction + (fine_fraction - base_fraction) * quality_factor;

        return diagonal * fraction;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STLExporter: Error calculating recommended deflection - " + String(e.GetMessageString()));
        return 0.1;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STLExporter: Error calculating recommended deflection - " + String(e.what()));
        return 0.1;
    }
}

void ocgd_STLExporter::_bind_methods() {
    // ASCII mode property
    ClassDB::bind_method(D_METHOD("set_ascii_mode", "ascii_mode"), &ocgd_STLExporter::set_ascii_mode);
    ClassDB::bind_method(D_METHOD("get_ascii_mode"), &ocgd_STLExporter::get_ascii_mode);
    ClassDB::add_property("ocgd_STLExporter", PropertyInfo(Variant::BOOL, "ascii_mode"), "set_ascii_mode", "get_ascii_mode");

    // Export methods
    ClassDB::bind_method(D_METHOD("write_file", "shape", "filename"), &ocgd_STLExporter::write_file);
    ClassDB::bind_method(D_METHOD("write_file_with_progress", "shape", "filename", "progress_callback"), &ocgd_STLExporter::write_file_with_progress, DEFVAL(""));
    ClassDB::bind_method(D_METHOD("quick_export", "shape", "filename", "ascii_mode"), &ocgd_STLExporter::quick_export, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("export_to_bytes", "shape", "ascii_mode"), &ocgd_STLExporter::export_to_bytes);
    ClassDB::bind_method(D_METHOD("export_multiple_shapes", "shapes", "base_filename", "ascii_mode"), &ocgd_STLExporter::export_multiple_shapes, DEFVAL(true));

    // Validation and analysis methods
    ClassDB::bind_method(D_METHOD("can_export_shape", "shape"), &ocgd_STLExporter::can_export_shape);
    ClassDB::bind_method(D_METHOD("validate_shape_for_export", "shape"), &ocgd_STLExporter::validate_shape_for_export);
    ClassDB::bind_method(D_METHOD("get_triangle_count", "shape"), &ocgd_STLExporter::get_triangle_count);
    ClassDB::bind_method(D_METHOD("get_estimated_file_size", "shape", "ascii_mode"), &ocgd_STLExporter::get_estimated_file_size);
    ClassDB::bind_method(D_METHOD("get_recommended_deflection", "shape", "quality_factor"), &ocgd_STLExporter::get_recommended_deflection, DEFVAL(0.5));
}
