/**
 * ocgd_STEPCAFControl_Reader.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE STEPCAFControl_Reader class.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_STEPCAFControl_Reader.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/STEPCAFControl_Reader.hxx>
#include <opencascade/TDocStd_Document.hxx>
#include <opencascade/TDocStd_Application.hxx>
#include <opencascade/XCAFApp_Application.hxx>
#include <opencascade/XCAFDoc_DocumentTool.hxx>
#include <opencascade/XCAFDoc_ShapeTool.hxx>
#include <opencascade/TDataStd_Name.hxx>
#include <opencascade/TDF_LabelSequence.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/Standard_Failure.hxx>

#include <sstream>

using namespace godot;

ocgd_STEPCAFControl_Reader::ocgd_STEPCAFControl_Reader() {
    try {
        _reader = new STEPCAFControl_Reader();
        _owns_reader = true;
        _document.Nullify();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to create reader - " + String(e.GetMessageString()));
        _reader = nullptr;
        _owns_reader = false;
        _document.Nullify();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to create reader - " + String(e.what()));
        _reader = nullptr;
        _owns_reader = false;
        _document.Nullify();
    }
}

ocgd_STEPCAFControl_Reader::~ocgd_STEPCAFControl_Reader() {
    try {
        if (_owns_reader && _reader != nullptr) {
            delete _reader;
            _reader = nullptr;
        }
    } catch (...) {
        // Destructor should not throw, just log the issue
        UtilityFunctions::printerr("STEPCAFControl_Reader: Warning - exception during destruction");
    }
}

void ocgd_STEPCAFControl_Reader::init() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot initialize - reader is null");
            return;
        }

        Handle(XSControl_WorkSession) WS = new XSControl_WorkSession;
        _reader->Init(WS, Standard_True);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Initialization failed - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Initialization failed - " + String(e.what()));
    }
}

int ocgd_STEPCAFControl_Reader::read_file(const String& filename) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot read file - reader is not initialized");
            return static_cast<int>(IFSelect_RetFail);
        }

        if (filename.is_empty()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot read file - filename is empty");
            return static_cast<int>(IFSelect_RetVoid);
        }

        std::string std_filename = filename.utf8().get_data();
        IFSelect_ReturnStatus status = _reader->ReadFile(std_filename.c_str());
        
        if (status != IFSelect_RetDone) {
            String error_msg = "STEPCAFControl_Reader: Failed to read file '" + filename + "' - ";
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

        return static_cast<int>(status);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception reading file '" + filename + "' - " + String(e.GetMessageString()));
        return static_cast<int>(IFSelect_RetFail);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception reading file '" + filename + "' - " + String(e.what()));
        return static_cast<int>(IFSelect_RetFail);
    }
}

int ocgd_STEPCAFControl_Reader::read_stream(const String& name, const PackedByteArray& data) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot read stream - reader is not initialized");
            return static_cast<int>(IFSelect_RetFail);
        }

        if (data.size() == 0) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot read stream - data is empty");
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
        
        IFSelect_ReturnStatus status = _reader->ReadStream(std_name.c_str(), stream);
        
        if (status != IFSelect_RetDone) {
            String error_msg = "STEPCAFControl_Reader: Failed to read stream '" + name + "' - ";
            switch (status) {
                case IFSelect_RetVoid:
                    error_msg += "No data or empty stream";
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

        return static_cast<int>(status);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception reading stream '" + name + "' - " + String(e.GetMessageString()));
        return static_cast<int>(IFSelect_RetFail);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception reading stream '" + name + "' - " + String(e.what()));
        return static_cast<int>(IFSelect_RetFail);
    }
}

int ocgd_STEPCAFControl_Reader::nb_roots_for_transfer() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get root count - reader is not initialized");
            return 0;
        }
        
        return _reader->NbRootsForTransfer();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting root count - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting root count - " + String(e.what()));
        return 0;
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_STEPCAFControl_Reader::transfer_one_root(int num) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot transfer root - reader is not initialized");
            return Ref<ocgd_TopoDS_Shape>();
        }

        if (num < 1) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Invalid root number - must be >= 1, got: " + String::num(num));
            return Ref<ocgd_TopoDS_Shape>();
        }

        int available_roots = nb_roots_for_transfer();
        if (num > available_roots) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Root number out of range - requested: " + String::num(num) + ", available: " + String::num(available_roots));
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create a new document for the transfer if needed
        if (_document.IsNull()) {
            Handle(TDocStd_Application) app = XCAFApp_Application::GetApplication();
            if (app.IsNull()) {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get XCAF application");
                return Ref<ocgd_TopoDS_Shape>();
            }
            app->NewDocument("MDTV-XCAF", _document);
            
            if (_document.IsNull()) {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to create XCAF document");
                return Ref<ocgd_TopoDS_Shape>();
            }
        }
        
        Message_ProgressRange progress;
        Standard_Boolean result = _reader->TransferOneRoot(num, _document, progress);
        
        if (result == Standard_False) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to transfer root " + String::num(num));
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Get the shape tool and find root shapes
        Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(_document->Main());
        if (shapeTool.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get shape tool from document");
            return Ref<ocgd_TopoDS_Shape>();
        }

        TDF_LabelSequence rootLabels;
        shapeTool->GetFreeShapes(rootLabels);
        
        if (rootLabels.Length() > 0) {
            TopoDS_Shape shape = shapeTool->GetShape(rootLabels.Value(1));
            if (!shape.IsNull()) {
                Ref<ocgd_TopoDS_Shape> result_shape = memnew(ocgd_TopoDS_Shape);
                result_shape->set_occt_shape(shape);
                return result_shape;
            } else {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Transferred shape is null");
            }
        } else {
            UtilityFunctions::printerr("STEPCAFControl_Reader: No root shapes found after transfer");
        }
        
        return Ref<ocgd_TopoDS_Shape>();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception transferring root " + String::num(num) + " - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception transferring root " + String::num(num) + " - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_STEPCAFControl_Reader::transfer() {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot transfer - reader is not initialized");
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create a new document for the transfer if needed
        if (_document.IsNull()) {
            Handle(TDocStd_Application) app = XCAFApp_Application::GetApplication();
            if (app.IsNull()) {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get XCAF application");
                return Ref<ocgd_TopoDS_Shape>();
            }
            app->NewDocument("MDTV-XCAF", _document);
            
            if (_document.IsNull()) {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to create XCAF document");
                return Ref<ocgd_TopoDS_Shape>();
            }
        }
        
        Message_ProgressRange progress;
        Standard_Boolean result = _reader->Transfer(_document, progress);
        
        if (result == Standard_False) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Transfer operation failed");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Get the shape tool and find root shapes
        Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(_document->Main());
        if (shapeTool.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get shape tool from document");
            return Ref<ocgd_TopoDS_Shape>();
        }

        TDF_LabelSequence rootLabels;
        shapeTool->GetFreeShapes(rootLabels);
        
        if (rootLabels.Length() > 0) {
            TopoDS_Shape shape = shapeTool->GetShape(rootLabels.Value(1));
            if (!shape.IsNull()) {
                Ref<ocgd_TopoDS_Shape> result_shape = memnew(ocgd_TopoDS_Shape);
                result_shape->set_occt_shape(shape);
                return result_shape;
            } else {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Transferred shape is null");
            }
        } else {
            UtilityFunctions::printerr("STEPCAFControl_Reader: No root shapes found after transfer");
        }
        
        return Ref<ocgd_TopoDS_Shape>();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception during transfer - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception during transfer - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Ref<ocgd_TopoDS_Shape> ocgd_STEPCAFControl_Reader::perform(const String& filename) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot perform - reader is not initialized");
            return Ref<ocgd_TopoDS_Shape>();
        }

        if (filename.is_empty()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot perform - filename is empty");
            return Ref<ocgd_TopoDS_Shape>();
        }

        std::string std_filename = filename.utf8().get_data();
        
        // Create a new document for the transfer
        Handle(TDocStd_Application) app = XCAFApp_Application::GetApplication();
        if (app.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get XCAF application");
            return Ref<ocgd_TopoDS_Shape>();
        }
        app->NewDocument("MDTV-XCAF", _document);
        
        if (_document.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to create XCAF document");
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        Message_ProgressRange progress;
        Standard_Boolean result = _reader->Perform(std_filename.c_str(), _document, progress);
        
        if (result == Standard_False) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Perform operation failed for file: " + filename);
            return Ref<ocgd_TopoDS_Shape>();
        }
        
        // Get the shape tool and find root shapes
        Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(_document->Main());
        if (shapeTool.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get shape tool from document");
            return Ref<ocgd_TopoDS_Shape>();
        }

        TDF_LabelSequence rootLabels;
        shapeTool->GetFreeShapes(rootLabels);
        
        if (rootLabels.Length() > 0) {
            TopoDS_Shape shape = shapeTool->GetShape(rootLabels.Value(1));
            if (!shape.IsNull()) {
                Ref<ocgd_TopoDS_Shape> result_shape = memnew(ocgd_TopoDS_Shape);
                result_shape->set_occt_shape(shape);
                return result_shape;
            } else {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Performed shape is null");
            }
        } else {
            UtilityFunctions::printerr("STEPCAFControl_Reader: No root shapes found after perform");
        }
        
        return Ref<ocgd_TopoDS_Shape>();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception during perform operation on '" + filename + "' - " + String(e.GetMessageString()));
        return Ref<ocgd_TopoDS_Shape>();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception during perform operation on '" + filename + "' - " + String(e.what()));
        return Ref<ocgd_TopoDS_Shape>();
    }
}

Array ocgd_STEPCAFControl_Reader::get_root_shapes() const {
    Array shapes;
    
    try {
        if (_document.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get root shapes - no document available");
            return shapes;
        }
        
        Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(_document->Main());
        if (shapeTool.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Failed to get shape tool from document");
            return shapes;
        }

        TDF_LabelSequence rootLabels;
        shapeTool->GetFreeShapes(rootLabels);
        
        for (Standard_Integer i = 1; i <= rootLabels.Length(); i++) {
            TopoDS_Shape shape = shapeTool->GetShape(rootLabels.Value(i));
            if (!shape.IsNull()) {
                Ref<ocgd_TopoDS_Shape> result_shape = memnew(ocgd_TopoDS_Shape);
                result_shape->set_occt_shape(shape);
                shapes.append(result_shape);
            } else {
                UtilityFunctions::printerr("STEPCAFControl_Reader: Warning - found null shape at index " + String::num(i));
            }
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception getting root shapes - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception getting root shapes - " + String(e.what()));
    }
    
    return shapes;
}

String ocgd_STEPCAFControl_Reader::get_main_label() const {
    try {
        if (_document.IsNull()) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get main label - no document available");
            return "";
        }
        
        TDF_Label mainLabel = _document->Main();
        Handle(TDataStd_Name) name;
        if (mainLabel.FindAttribute(TDataStd_Name::GetID(), name)) {
            TCollection_ExtendedString extName = name->Get();
            return String(extName.ToExtString());
        }
        
        return "Main";

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception getting main label - " + String(e.GetMessageString()));
        return "";
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Exception getting main label - " + String(e.what()));
        return "";
    }
}

void ocgd_STEPCAFControl_Reader::set_color_mode(bool colormode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set color mode - reader is not initialized");
            return;
        }
        _reader->SetColorMode(colormode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting color mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting color mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_color_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get color mode - reader is not initialized");
            return false;
        }
        return _reader->GetColorMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting color mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting color mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_name_mode(bool namemode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set name mode - reader is not initialized");
            return;
        }
        _reader->SetNameMode(namemode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting name mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting name mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_name_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get name mode - reader is not initialized");
            return false;
        }
        return _reader->GetNameMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting name mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting name mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_layer_mode(bool layermode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set layer mode - reader is not initialized");
            return;
        }
        _reader->SetLayerMode(layermode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting layer mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting layer mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_layer_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get layer mode - reader is not initialized");
            return false;
        }
        return _reader->GetLayerMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting layer mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting layer mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_props_mode(bool propsmode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set props mode - reader is not initialized");
            return;
        }
        _reader->SetPropsMode(propsmode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting props mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting props mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_props_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get props mode - reader is not initialized");
            return false;
        }
        return _reader->GetPropsMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting props mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting props mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_meta_mode(bool metamode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set meta mode - reader is not initialized");
            return;
        }
        _reader->SetMetaMode(metamode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting meta mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting meta mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_meta_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get meta mode - reader is not initialized");
            return false;
        }
        return _reader->GetMetaMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting meta mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting meta mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_product_meta_mode(bool productmetamode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set product meta mode - reader is not initialized");
            return;
        }
        _reader->SetProductMetaMode(productmetamode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting product meta mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting product meta mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_product_meta_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get product meta mode - reader is not initialized");
            return false;
        }
        return _reader->GetProductMetaMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting product meta mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting product meta mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_shuo_mode(bool shuomode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set SHUO mode - reader is not initialized");
            return;
        }
        _reader->SetSHUOMode(shuomode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting SHUO mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting SHUO mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_shuo_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get SHUO mode - reader is not initialized");
            return false;
        }
        return _reader->GetSHUOMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting SHUO mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting SHUO mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_gdt_mode(bool gdtmode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set GDT mode - reader is not initialized");
            return;
        }
        _reader->SetGDTMode(gdtmode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting GDT mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting GDT mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_gdt_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get GDT mode - reader is not initialized");
            return false;
        }
        return _reader->GetGDTMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting GDT mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting GDT mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_mat_mode(bool matmode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set material mode - reader is not initialized");
            return;
        }
        _reader->SetMatMode(matmode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting material mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting material mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_mat_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get material mode - reader is not initialized");
            return false;
        }
        return _reader->GetMatMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting material mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting material mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::set_view_mode(bool viewmode) {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot set view mode - reader is not initialized");
            return;
        }
        _reader->SetViewMode(viewmode ? Standard_True : Standard_False);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting view mode - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error setting view mode - " + String(e.what()));
    }
}

bool ocgd_STEPCAFControl_Reader::get_view_mode() const {
    try {
        if (_reader == nullptr) {
            UtilityFunctions::printerr("STEPCAFControl_Reader: Cannot get view mode - reader is not initialized");
            return false;
        }
        return _reader->GetViewMode() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting view mode - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("STEPCAFControl_Reader: Error getting view mode - " + String(e.what()));
        return false;
    }
}

void ocgd_STEPCAFControl_Reader::_bind_methods() {
    // Constructor and setup methods
    ClassDB::bind_method(D_METHOD("init"), &ocgd_STEPCAFControl_Reader::init);
    
    // File I/O methods
    ClassDB::bind_method(D_METHOD("read_file", "filename"), &ocgd_STEPCAFControl_Reader::read_file);
    ClassDB::bind_method(D_METHOD("read_stream", "name", "data"), &ocgd_STEPCAFControl_Reader::read_stream);
    ClassDB::bind_method(D_METHOD("nb_roots_for_transfer"), &ocgd_STEPCAFControl_Reader::nb_roots_for_transfer);
    
    // Transfer methods
    ClassDB::bind_method(D_METHOD("transfer_one_root", "num"), &ocgd_STEPCAFControl_Reader::transfer_one_root);
    ClassDB::bind_method(D_METHOD("transfer"), &ocgd_STEPCAFControl_Reader::transfer);
    ClassDB::bind_method(D_METHOD("perform", "filename"), &ocgd_STEPCAFControl_Reader::perform);
    
    // Shape access methods
    ClassDB::bind_method(D_METHOD("get_root_shapes"), &ocgd_STEPCAFControl_Reader::get_root_shapes);
    ClassDB::bind_method(D_METHOD("get_main_label"), &ocgd_STEPCAFControl_Reader::get_main_label);
    
    // Mode setters and getters
    ClassDB::bind_method(D_METHOD("set_color_mode", "colormode"), &ocgd_STEPCAFControl_Reader::set_color_mode);
    ClassDB::bind_method(D_METHOD("get_color_mode"), &ocgd_STEPCAFControl_Reader::get_color_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "color_mode"), "set_color_mode", "get_color_mode");
    
    ClassDB::bind_method(D_METHOD("set_name_mode", "namemode"), &ocgd_STEPCAFControl_Reader::set_name_mode);
    ClassDB::bind_method(D_METHOD("get_name_mode"), &ocgd_STEPCAFControl_Reader::get_name_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "name_mode"), "set_name_mode", "get_name_mode");
    
    ClassDB::bind_method(D_METHOD("set_layer_mode", "layermode"), &ocgd_STEPCAFControl_Reader::set_layer_mode);
    ClassDB::bind_method(D_METHOD("get_layer_mode"), &ocgd_STEPCAFControl_Reader::get_layer_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "layer_mode"), "set_layer_mode", "get_layer_mode");
    
    ClassDB::bind_method(D_METHOD("set_props_mode", "propsmode"), &ocgd_STEPCAFControl_Reader::set_props_mode);
    ClassDB::bind_method(D_METHOD("get_props_mode"), &ocgd_STEPCAFControl_Reader::get_props_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "props_mode"), "set_props_mode", "get_props_mode");
    
    ClassDB::bind_method(D_METHOD("set_meta_mode", "metamode"), &ocgd_STEPCAFControl_Reader::set_meta_mode);
    ClassDB::bind_method(D_METHOD("get_meta_mode"), &ocgd_STEPCAFControl_Reader::get_meta_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "meta_mode"), "set_meta_mode", "get_meta_mode");
    
    ClassDB::bind_method(D_METHOD("set_product_meta_mode", "productmetamode"), &ocgd_STEPCAFControl_Reader::set_product_meta_mode);
    ClassDB::bind_method(D_METHOD("get_product_meta_mode"), &ocgd_STEPCAFControl_Reader::get_product_meta_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "product_meta_mode"), "set_product_meta_mode", "get_product_meta_mode");
    
    ClassDB::bind_method(D_METHOD("set_shuo_mode", "shuomode"), &ocgd_STEPCAFControl_Reader::set_shuo_mode);
    ClassDB::bind_method(D_METHOD("get_shuo_mode"), &ocgd_STEPCAFControl_Reader::get_shuo_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "shuo_mode"), "set_shuo_mode", "get_shuo_mode");
    
    ClassDB::bind_method(D_METHOD("set_gdt_mode", "gdtmode"), &ocgd_STEPCAFControl_Reader::set_gdt_mode);
    ClassDB::bind_method(D_METHOD("get_gdt_mode"), &ocgd_STEPCAFControl_Reader::get_gdt_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "gdt_mode"), "set_gdt_mode", "get_gdt_mode");
    
    ClassDB::bind_method(D_METHOD("set_mat_mode", "matmode"), &ocgd_STEPCAFControl_Reader::set_mat_mode);
    ClassDB::bind_method(D_METHOD("get_mat_mode"), &ocgd_STEPCAFControl_Reader::get_mat_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "mat_mode"), "set_mat_mode", "get_mat_mode");
    
    ClassDB::bind_method(D_METHOD("set_view_mode", "viewmode"), &ocgd_STEPCAFControl_Reader::set_view_mode);
    ClassDB::bind_method(D_METHOD("get_view_mode"), &ocgd_STEPCAFControl_Reader::get_view_mode);
    ClassDB::add_property("ocgd_STEPCAFControl_Reader", PropertyInfo(Variant::BOOL, "view_mode"), "set_view_mode", "get_view_mode");
}