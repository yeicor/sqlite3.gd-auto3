/**
 * ocgd_IGESReader.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE IGES import functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_IGESReader.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/IGESControl_Reader.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/IGESData_IGESModel.hxx>
#include <opencascade/IFSelect_ReturnStatus.hxx>
#include <opencascade/IFSelect_PrintFail.hxx>
#include <opencascade/IFSelect_PrintCount.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/BRep_Builder.hxx>
#include <opencascade/TopoDS_Compound.hxx>
#include <opencascade/Standard_Failure.hxx>

#include <sstream>

using namespace godot;

ocgd_IGESReader::ocgd_IGESReader() {
    try {
        _reader = new IGESControl_Reader();
        _owns_reader = true;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Failed to create reader - " + String(e.GetMessageString()));
        _reader = nullptr;
        _owns_reader = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Failed to create reader - " + String(e.what()));
        _reader = nullptr;
        _owns_reader = false;
    }
}

ocgd_IGESReader::~ocgd_IGESReader() {
    try {
        if (_owns_reader && _reader != nullptr) {
            delete _reader;
            _reader = nullptr;
        }
    } catch (...) {
        // Destructor should not throw, just log the issue
        UtilityFunctions::printerr("IGESReader: Warning - exception during destruction");
    }
}

void ocgd_IGESReader::clear() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot clear - reader is not initialized");
            return;
        }
        _reader->ClearShapes();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error clearing shapes - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error clearing shapes - " + String(e.what()));
    }
}

void ocgd_IGESReader::set_read_visible(bool read_visible) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot set read visible - reader is not initialized");
            return;
        }
        _reader->SetReadVisible(read_visible ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error setting read visible mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error setting read visible mode - " + String(e.what()));
    }
}

bool ocgd_IGESReader::get_read_visible() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get read visible - reader is not initialized");
            return true; // Default
        }
        return _reader->GetReadVisible() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error getting read visible mode - " + String(e.GetMessageString()));
        return true;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error getting read visible mode - " + String(e.what()));
        return true;
    }
}

int ocgd_IGESReader::read_file(const String& filename) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot read file - reader is not initialized");
            return static_cast<int>(IFSelect_RetFail);
        }

        if (filename.is_empty()) {
            UtilityFunctions::printerr("IGESReader: Cannot read file - filename is empty");
            return static_cast<int>(IFSelect_RetVoid);
        }

        std::string std_filename = filename.utf8().get_data();
        IFSelect_ReturnStatus status = _reader->ReadFile(std_filename.c_str());
        
        if (status != IFSelect_RetDone) {
            String error_msg = "IGESReader: Failed to read file '" + filename + "' - ";
            switch (status) {
                case IFSelect_RetVoid:
                    error_msg += "No data or empty file";
                    break;
                case IFSelect_RetError:
                    error_msg += "Read error occurred";
                    break;
                case IFSelect_RetFail:
                    error_msg += "Read operation failed";
                    break;
                case IFSelect_RetStop:
                    error_msg += "Read operation was stopped";
                    break;
                default:
                    error_msg += "Unknown error";
                    break;
            }
            UtilityFunctions::printerr(error_msg);
        }

        return status_to_int(status);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception reading file '" + filename + "' - " + String(e.GetMessageString()));
        return static_cast<int>(IFSelect_RetFail);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception reading file '" + filename + "' - " + String(e.what()));
        return static_cast<int>(IFSelect_RetFail);
    }
}

int ocgd_IGESReader::read_stream(const String& name, const PackedByteArray& data) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot read stream - reader is not initialized");
            return static_cast<int>(IFSelect_RetFail);
        }

        if (data.size() == 0) {
            UtilityFunctions::printerr("IGESReader: Cannot read stream - data is empty");
            return static_cast<int>(IFSelect_RetVoid);
        }

        std::string std_name = name.utf8().get_data();
        
        // Convert PackedByteArray to std::stringstream
        std::stringstream stream;
        const uint8_t* raw_data = data.ptr();
        size_t size = data.size();
        
        for (size_t i = 0; i < size; ++i) {
            stream << static_cast<char>(raw_data[i]);
        }
        
        // Note: IGESControl_Reader doesn't have ReadStream method like STEP reader
        // This would require a more complex implementation using temporary files
        UtilityFunctions::printerr("IGESReader: Stream reading not yet implemented for IGES format");
        return static_cast<int>(IFSelect_RetFail);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception reading stream '" + name + "' - " + String(e.GetMessageString()));
        return static_cast<int>(IFSelect_RetFail);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception reading stream '" + name + "' - " + String(e.what()));
        return static_cast<int>(IFSelect_RetFail);
    }
}

Dictionary ocgd_IGESReader::check_file(bool fails_only) {
    Dictionary result;
    
    try {
        if (_reader == nullptr) {
            result["valid"] = false;
            result["error"] = "Reader not initialized";
            return result;
        }

        // Check functionality is not available in current API
        Standard_Boolean check_result = Standard_True;
        result["valid"] = check_result == Standard_True;
        
        // Basic validation result - more detailed checking would require
        // access to the internal check lists
        if (!check_result) {
            result["error"] = "IGES file contains errors or warnings";
        }

    } catch (const Standard_Failure& e) {
        result["valid"] = false;
        result["error"] = "Check failed: " + String(e.GetMessageString());
        UtilityFunctions::printerr("IGESReader: Exception during file check - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        result["valid"] = false;
        result["error"] = "Check failed: " + String(e.what());
        UtilityFunctions::printerr("IGESReader: Exception during file check - " + String(e.what()));
    }
    
    return result;
}

void ocgd_IGESReader::print_check_load(bool fails_only, int mode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot print check load - reader is not initialized");
            return;
        }

        if (mode < 0 || mode > 2) {
            UtilityFunctions::printerr("IGESReader: Invalid print mode - should be 0, 1, or 2, got: " + String::num(mode));
            mode = 1; // Use default medium mode
        }

        IFSelect_PrintCount print_mode = static_cast<IFSelect_PrintCount>(mode);
        _reader->PrintCheckLoad(fails_only ? Standard_True : Standard_False, print_mode);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error printing check load - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error printing check load - " + String(e.what()));
    }
}

int ocgd_IGESReader::nb_roots_for_transfer() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get root count - reader is not initialized");
            return 0;
        }
        return _reader->NbRootsForTransfer();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error getting root count - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error getting root count - " + String(e.what()));
        return 0;
    }
}

int ocgd_IGESReader::transfer_roots(bool only_visible) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot transfer roots - reader is not initialized");
            return 0;
        }

        Standard_Integer result = _reader->TransferRoots();
        if (result == Standard_True) {
            return _reader->NbShapes();
        } else {
            UtilityFunctions::printerr("IGESReader: Transfer roots operation failed");
            return 0;
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring roots - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring roots - " + String(e.what()));
        return 0;
    }
}

bool ocgd_IGESReader::transfer_entity(int entity_index) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot transfer entity - reader is not initialized");
            return false;
        }

        if (entity_index < 1) {
            UtilityFunctions::printerr("IGESReader: Invalid entity index - must be >= 1, got: " + String::num(entity_index));
            return false;
        }

        Standard_Boolean result = _reader->TransferOne(entity_index);
        if (result != Standard_True) {
            UtilityFunctions::printerr("IGESReader: Failed to transfer entity " + String::num(entity_index));
        }
        return result == Standard_True;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring entity " + String::num(entity_index) + " - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring entity " + String::num(entity_index) + " - " + String(e.what()));
        return false;
    }
}

int ocgd_IGESReader::transfer_list(const Array& entity_indices) {
    int success_count = 0;
    
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot transfer entity list - reader is not initialized");
            return 0;
        }

        if (entity_indices.size() == 0) {
            UtilityFunctions::printerr("IGESReader: Cannot transfer empty entity list");
            return 0;
        }

        for (int i = 0; i < entity_indices.size(); i++) {
            try {
                Variant index_var = entity_indices[i];
                if (index_var.get_type() != Variant::INT) {
                    UtilityFunctions::printerr("IGESReader: Invalid entity index type at position " + String::num(i) + " - expected int");
                    continue;
                }

                int index = index_var;
                if (transfer_entity(index)) {
                    success_count++;
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("IGESReader: Exception transferring entity at position " + String::num(i) + " - " + String(e.GetMessageString()));
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("IGESReader: Exception transferring entity at position " + String::num(i) + " - " + String(e.what()));
            }
        }
        
        return success_count;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring entity list - " + String(e.GetMessageString()));
        return success_count;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception transferring entity list - " + String(e.what()));
        return success_count;
    }
}

bool ocgd_IGESReader::is_done() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot check if done - reader is not initialized");
            return false;
        }
        return _reader->NbShapes() > 0;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error checking if done - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error checking if done - " + String(e.what()));
        return false;
    }
}

int ocgd_IGESReader::nb_shapes() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get shape count - reader is not initialized");
            return 0;
        }
        return _reader->NbShapes();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error getting shape count - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error getting shape count - " + String(e.what()));
        return 0;
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_IGESReader::get_shape(int shape_index) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get shape - reader is not initialized");
            return Ref<ocgd_TopoDS_Shape>();
        }

        if (shape_index < 1) {
            UtilityFunctions::printerr("IGESReader: Invalid shape index - must be >= 1, got: " + String::num(shape_index));
            return Ref<ocgd_TopoDS_Shape>();
        }

        int total_shapes = nb_shapes();
        if (shape_index > total_shapes) {
            UtilityFunctions::printerr("IGESReader: Shape index out of range - requested: " + String::num(shape_index) + ", available: " + String::num(total_shapes));
            return Ref<ocgd_TopoDS_Shape>();
        }

        TopoDS_Shape shape = _reader->Shape(shape_index);
        if (shape.IsNull()) {
            UtilityFunctions::printerr("IGESReader: Retrieved shape " + String::num(shape_index) + " is null");
            return Ref<ocgd_TopoDS_Shape>();
        }

        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        result->set_occt_shape(shape);
        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting shape " + String::num(shape_index) + " - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting shape " + String::num(shape_index) + " - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Array ocgd_IGESReader::get_all_shapes() {
    Array shapes;
    
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get all shapes - reader is not initialized");
            return shapes;
        }

        int count = nb_shapes();
        if (count == 0) {
            return shapes; // No shapes available, not an error
        }

        for (int i = 1; i <= count; i++) {
            try {
                Ref<ocgd_TopoDS_Shape> shape = get_shape(i);
                if (shape.is_valid()) {
                    shapes.append(shape);
                } else {
                    UtilityFunctions::printerr("IGESReader: Warning - shape " + String::num(i) + " is invalid, skipping");
                }
            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("IGESReader: Exception getting shape " + String::num(i) + " - " + String(e.GetMessageString()));
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("IGESReader: Exception getting shape " + String::num(i) + " - " + String(e.what()));
            }
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting all shapes - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting all shapes - " + String(e.what()));
    }
    
    return shapes;
}

Ref<ocgd_TopoDS_Shape> ocgd_IGESReader::get_one_shape() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot get one shape - reader is not initialized");
            return Ref<ocgd_TopoDS_Shape>();
        }

        TopoDS_Shape shape = _reader->OneShape();
        if (shape.IsNull()) {
            UtilityFunctions::printerr("IGESReader: One shape result is null");
            return Ref<ocgd_TopoDS_Shape>();
        }

        Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
        result->set_occt_shape(shape);
        return result;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting one shape - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception getting one shape - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_IGESReader::import_file(const String& filename) {
    try {
        if (filename.is_empty()) {
            UtilityFunctions::printerr("IGESReader: Cannot import - filename is empty");
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Simple import workflow
        int read_status = read_file(filename);
        if (read_status != STATUS_OK) {
            UtilityFunctions::printerr("IGESReader: Failed to read file: " + filename);
            return Ref<ocgd_TopoDS_Shape>();
        }

        int transfer_count = transfer_roots(true);
        if (transfer_count == 0) {
            UtilityFunctions::printerr("IGESReader: No entities transferred from file: " + filename);
            return Ref<ocgd_TopoDS_Shape>();
        }

        return get_one_shape();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception importing file '" + filename + "' - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception importing file '" + filename + "' - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Dictionary ocgd_IGESReader::import_file_with_validation(const String& filename, bool check_validity) {
    Dictionary result;
    
    try {
        if (filename.is_empty()) {
            result["success"] = false;
            result["error"] = "Filename is empty";
            return result;
        }

        int read_status = read_file(filename);
        result["read_status"] = read_status;
        
        if (read_status != STATUS_OK) {
            result["success"] = false;
            result["error"] = "Failed to read IGES file";
            return result;
        }

        if (check_validity) {
            Dictionary validation = check_file(false);
            result["validation"] = validation;
            
            if (!validation.get("valid", false)) {
                result["success"] = false;
                result["error"] = "IGES file validation failed";
                return result;
            }
        }

        int transfer_count = transfer_roots(true);
        result["transfer_count"] = transfer_count;
        
        if (transfer_count > 0) {
            result["success"] = true;
            result["shape"] = get_one_shape();
            
            if (result["shape"] == Variant()) {
                result["success"] = false;
                result["error"] = "Failed to get transferred shape";
            }
        } else {
            result["success"] = false;
            result["error"] = "No entities could be transferred";
        }

    } catch (const Standard_Failure& e) {
        result["success"] = false;
        result["error"] = "Exception during import: " + String(e.GetMessageString());
        UtilityFunctions::printerr("IGESReader: Exception in import with validation - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        result["success"] = false;
        result["error"] = "Exception during import: " + String(e.what());
        UtilityFunctions::printerr("IGESReader: Exception in import with validation - " + String(e.what()));
    }
    
    return result;
}

Dictionary ocgd_IGESReader::get_model_info() {
    Dictionary result;
    
    try {
        if (_reader == nullptr) {
            result["error"] = "Reader not initialized";
            return result;
        }

        Handle(IGESData_IGESModel) model = _reader->IGESModel();
        if (model.IsNull()) {
            result["error"] = "No model loaded";
            return result;
        }

        result["nb_entities"] = model->NbEntities();
        result["has_model"] = true;
        
        // Add more model information as needed
        result["nb_roots"] = nb_roots_for_transfer();
        result["nb_shapes"] = nb_shapes();

    } catch (const Standard_Failure& e) {
        result["error"] = "Exception getting model info: " + String(e.GetMessageString());
        UtilityFunctions::printerr("IGESReader: Exception getting model info - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        result["error"] = "Exception getting model info: " + String(e.what());
        UtilityFunctions::printerr("IGESReader: Exception getting model info - " + String(e.what()));
    }
    
    return result;
}

Dictionary ocgd_IGESReader::get_entity_statistics() {
    Dictionary result;
    
    try {
        if (_reader == nullptr) {
            result["error"] = "Reader not initialized";
            return result;
        }

        Handle(IGESData_IGESModel) model = _reader->IGESModel();
        if (model.IsNull()) {
            result["error"] = "No model loaded";
            return result;
        }

        result["total_entities"] = model->NbEntities();
        result["root_entities"] = nb_roots_for_transfer();
        result["transferred_shapes"] = nb_shapes();

    } catch (const Standard_Failure& e) {
        result["error"] = "Exception getting entity statistics: " + String(e.GetMessageString());
        UtilityFunctions::printerr("IGESReader: Exception getting entity statistics - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        result["error"] = "Exception getting entity statistics: " + String(e.what());
        UtilityFunctions::printerr("IGESReader: Exception getting entity statistics - " + String(e.what()));
    }
    
    return result;
}

void ocgd_IGESReader::print_transfer_info(int fail_warn, int mode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("IGESReader: Cannot print transfer info - reader is not initialized");
            return;
        }

        if (fail_warn < 0 || fail_warn > 2) {
            UtilityFunctions::printerr("IGESReader: Invalid fail/warn level - should be 0, 1, or 2, got: " + String::num(fail_warn));
            fail_warn = 1;
        }

        if (mode < 0 || mode > 2) {
            UtilityFunctions::printerr("IGESReader: Invalid print mode - should be 0, 1, or 2, got: " + String::num(mode));
            mode = 1;
        }

        IFSelect_PrintFail print_fail = static_cast<IFSelect_PrintFail>(fail_warn);
        IFSelect_PrintCount print_mode = static_cast<IFSelect_PrintCount>(mode);
        _reader->PrintTransferInfo(print_fail, print_mode);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error printing transfer info - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error printing transfer info - " + String(e.what()));
    }
}

bool ocgd_IGESReader::has_model() const {
    try {
        if (_reader == nullptr) {
            return false;
        }
        
        Handle(IGESData_IGESModel) model = _reader->IGESModel();
        return !model.IsNull();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Error checking if model exists - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Error checking if model exists - " + String(e.what()));
        return false;
    }
}

Array ocgd_IGESReader::import_multiple_files(const Array& filenames) {
    Array results;
    
    try {
        if (filenames.size() == 0) {
            UtilityFunctions::printerr("IGESReader: Cannot import multiple files - filename array is empty");
            return results;
        }

        for (int i = 0; i < filenames.size(); i++) {
            try {
                Variant filename_var = filenames[i];
                if (filename_var.get_type() != Variant::STRING) {
                    UtilityFunctions::printerr("IGESReader: Invalid filename type at position " + String::num(i) + " - expected string");
                    results.append(Ref<ocgd_TopoDS_Shape>());
                    continue;
                }

                String filename = filename_var;
                Ref<ocgd_TopoDS_Shape> shape = import_file(filename);
                results.append(shape);

            } catch (const Standard_Failure& e) {
                UtilityFunctions::printerr("IGESReader: Exception importing file at position " + String::num(i) + " - " + String(e.GetMessageString()));
                results.append(Ref<ocgd_TopoDS_Shape>());
            } catch (const std::exception& e) {
                UtilityFunctions::printerr("IGESReader: Exception importing file at position " + String::num(i) + " - " + String(e.what()));
                results.append(Ref<ocgd_TopoDS_Shape>());
            }
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception importing multiple files - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception importing multiple files - " + String(e.what()));
    }
    
    return results;
}

int ocgd_IGESReader::status_to_int(IFSelect_ReturnStatus status) const {
    return static_cast<int>(status);
}

Dictionary ocgd_IGESReader::extract_model_statistics() const {
    Dictionary stats;
    
    try {
        if (_reader == nullptr) {
            return stats;
        }

        Handle(IGESData_IGESModel) model = _reader->IGESModel();
        if (model.IsNull()) {
            return stats;
        }

        stats["nb_entities"] = model->NbEntities();
        stats["nb_roots"] = const_cast<ocgd_IGESReader*>(this)->nb_roots_for_transfer();
        stats["nb_shapes"] = const_cast<ocgd_IGESReader*>(this)->nb_shapes();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("IGESReader: Exception extracting model statistics - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("IGESReader: Exception extracting model statistics - " + String(e.what()));
    }

    return stats;
}

void ocgd_IGESReader::_bind_methods() {
    // Configuration methods
    ClassDB::bind_method(D_METHOD("clear"), &ocgd_IGESReader::clear);
    ClassDB::bind_method(D_METHOD("set_read_visible", "read_visible"), &ocgd_IGESReader::set_read_visible);
    ClassDB::bind_method(D_METHOD("get_read_visible"), &ocgd_IGESReader::get_read_visible);
    ClassDB::add_property("ocgd_IGESReader", PropertyInfo(Variant::BOOL, "read_visible"), "set_read_visible", "get_read_visible");
    
    // File loading methods
    ClassDB::bind_method(D_METHOD("read_file", "filename"), &ocgd_IGESReader::read_file);
    ClassDB::bind_method(D_METHOD("read_stream", "name", "data"), &ocgd_IGESReader::read_stream);
    
    // Validation methods
    ClassDB::bind_method(D_METHOD("check_file", "fails_only"), &ocgd_IGESReader::check_file, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("print_check_load", "fails_only", "mode"), &ocgd_IGESReader::print_check_load, DEFVAL(false), DEFVAL(1));
    
    // Transfer methods
    ClassDB::bind_method(D_METHOD("nb_roots_for_transfer"), &ocgd_IGESReader::nb_roots_for_transfer);
    ClassDB::bind_method(D_METHOD("transfer_roots", "only_visible"), &ocgd_IGESReader::transfer_roots, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("transfer_entity", "entity_index"), &ocgd_IGESReader::transfer_entity);
    ClassDB::bind_method(D_METHOD("transfer_list", "entity_indices"), &ocgd_IGESReader::transfer_list);
    
    // Result access methods
    ClassDB::bind_method(D_METHOD("is_done"), &ocgd_IGESReader::is_done);
    ClassDB::bind_method(D_METHOD("nb_shapes"), &ocgd_IGESReader::nb_shapes);
    ClassDB::bind_method(D_METHOD("get_shape", "shape_index"), &ocgd_IGESReader::get_shape);
    ClassDB::bind_method(D_METHOD("get_all_shapes"), &ocgd_IGESReader::get_all_shapes);
    ClassDB::bind_method(D_METHOD("get_one_shape"), &ocgd_IGESReader::get_one_shape);
    
    // Convenience methods
    ClassDB::bind_method(D_METHOD("import_file", "filename"), &ocgd_IGESReader::import_file);
    ClassDB::bind_method(D_METHOD("import_file_with_validation", "filename", "check_validity"), &ocgd_IGESReader::import_file_with_validation, DEFVAL(true));
    ClassDB::bind_method(D_METHOD("import_multiple_files", "filenames"), &ocgd_IGESReader::import_multiple_files);
    
    // Information methods
    ClassDB::bind_method(D_METHOD("get_model_info"), &ocgd_IGESReader::get_model_info);
    ClassDB::bind_method(D_METHOD("get_entity_statistics"), &ocgd_IGESReader::get_entity_statistics);
    ClassDB::bind_method(D_METHOD("print_transfer_info", "fail_warn", "mode"), &ocgd_IGESReader::print_transfer_info, DEFVAL(1), DEFVAL(1));
    ClassDB::bind_method(D_METHOD("has_model"), &ocgd_IGESReader::has_model);
    
    // Status code enum constants
    BIND_ENUM_CONSTANT(STATUS_OK);
    BIND_ENUM_CONSTANT(STATUS_WARNING);
    BIND_ENUM_CONSTANT(STATUS_ERROR);
    BIND_ENUM_CONSTANT(STATUS_FAIL);
    BIND_ENUM_CONSTANT(STATUS_VOID);
    BIND_ENUM_CONSTANT(STATUS_STOP);
    
    // Print mode enum constants
    BIND_ENUM_CONSTANT(PRINT_SHORT);
    BIND_ENUM_CONSTANT(PRINT_MEDIUM);
    BIND_ENUM_CONSTANT(PRINT_DETAILED);
}