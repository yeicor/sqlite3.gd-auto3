/**
 * ocgd_BRepMesh_IncrementalMesh.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE BRepMesh_IncrementalMesh class.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_BRepMesh_IncrementalMesh.hxx"
#include "ocgd_EnhancedNormals.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/BRepMesh_IncrementalMesh.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/IMeshTools_Parameters.hxx>
#include <opencascade/Message_ProgressRange.hxx>
#include <opencascade/Standard_Failure.hxx>
#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/BRep_Tool.hxx>

using namespace godot;

ocgd_BRepMesh_IncrementalMesh::ocgd_BRepMesh_IncrementalMesh() : _compute_normals(true) {
    try {
        _mesh = new BRepMesh_IncrementalMesh();
        _owns_mesh = true;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to create mesh - " + String(e.GetMessageString()));
        _mesh = nullptr;
        _owns_mesh = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to create mesh - " + String(e.what()));
        _mesh = nullptr;
        _owns_mesh = false;
    }
}

ocgd_BRepMesh_IncrementalMesh::~ocgd_BRepMesh_IncrementalMesh() {
    try {
        if (_owns_mesh && _mesh != nullptr) {
            delete _mesh;
            _mesh = nullptr;
        }
    } catch (...) {
        // Destructor should not throw, just log the issue
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Warning - exception during destruction");
    }
}

void ocgd_BRepMesh_IncrementalMesh::init_with_shape(const Ref<ocgd_TopoDS_Shape>& shape,
                                                   double linear_deflection,
                                                   bool is_relative,
                                                   double angular_deflection,
                                                   bool is_in_parallel,
                                                   bool compute_normals) {
    try {
        if (shape.is_null()) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot initialize with null shape reference");
            return;
        }

        if (shape->is_null()) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot initialize with null shape");
            return;
        }

        // Validate parameters
        if (linear_deflection <= 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Linear deflection must be positive, got: " + String::num(linear_deflection));
            return;
        }

        if (angular_deflection <= 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Angular deflection must be positive, got: " + String::num(angular_deflection));
            return;
        }

        if (!std::isfinite(linear_deflection) || !std::isfinite(angular_deflection)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Deflection values must be finite");
            return;
        }

        // Clean up existing mesh if owned
        if (_owns_mesh && _mesh != nullptr) {
            delete _mesh;
        }

        // Store shape reference and settings for normal computation
        _shape = shape;
        _compute_normals = compute_normals;

        // Create new mesh with shape
        _mesh = new BRepMesh_IncrementalMesh(
            shape->get_occt_shape(),
            linear_deflection,
            is_relative ? Standard_True : Standard_False,
            angular_deflection,
            is_in_parallel ? Standard_True : Standard_False
        );
        _owns_mesh = true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to initialize with shape - " + String(e.GetMessageString()));
        if (_owns_mesh && _mesh != nullptr) {
            delete _mesh;
        }
        _mesh = nullptr;
        _owns_mesh = false;
        _shape.unref();
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to initialize with shape - " + String(e.what()));
        if (_owns_mesh && _mesh != nullptr) {
            delete _mesh;
        }
        _mesh = nullptr;
        _owns_mesh = false;
        _shape.unref();
    }
}

void ocgd_BRepMesh_IncrementalMesh::perform() {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot perform meshing - mesh is null");
            return;
        }

        _mesh->Perform();
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Meshing performed successfully");

        // Automatically compute normals if requested
        if (_compute_normals) {
            compute_normals();
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Exception during meshing - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Exception during meshing - " + String(e.what()));
    }
}

void ocgd_BRepMesh_IncrementalMesh::compute_normals() {
    try {
        if (_shape.is_null() || _shape->is_null()) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot compute normals - no shape available");
            return;
        }

        const TopoDS_Shape& occt_shape = _shape->get_occt_shape();
        if (occt_shape.IsNull()) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot compute normals - OpenCASCADE shape is null");
            return;
        }

        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Computing normals for meshed shape using enhanced algorithms");
        
        int faces_processed = 0;
        int faces_with_surface_normals = 0;
        int faces_with_triangle_normals = 0;
        
        TopExp_Explorer face_explorer(occt_shape, TopAbs_FACE);
        while (face_explorer.More()) {
            const TopoDS_Face& face = TopoDS::Face(face_explorer.Current());
            TopLoc_Location location;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, location);

            if (!triangulation.IsNull() && !triangulation->HasNormals()) {
                const Standard_Integer nb_nodes = triangulation->NbNodes();
                const Standard_Integer nb_triangles = triangulation->NbTriangles();

                if (nb_nodes > 0 && nb_triangles > 0) {
                    // Use enhanced normal computation
                    if (ocgd_EnhancedNormals::compute_and_store_normals(face, triangulation)) {
                        faces_processed++;
                        
                        // Get diagnostic info to track computation method
                        Dictionary info = ocgd_EnhancedNormals::get_normal_computation_info(face, triangulation);
                        String method = info.get("computation_method", "unknown");
                        if (method == "surface_based") {
                            faces_with_surface_normals++;
                        } else if (method == "triangle_based") {
                            faces_with_triangle_normals++;
                        }
                    } else {
                        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to compute normals for face, skipping");
                    }
                }
            }

            face_explorer.Next();
        }

        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Enhanced normal computation completed for " + 
                                 String::num(faces_processed) + " faces (" +
                                 String::num(faces_with_surface_normals) + " surface-based, " +
                                 String::num(faces_with_triangle_normals) + " triangle-based)");

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to compute normals - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Failed to compute normals - " + String(e.what()));
    }
}

bool ocgd_BRepMesh_IncrementalMesh::get_compute_normals() const {
    return _compute_normals;
}

void ocgd_BRepMesh_IncrementalMesh::set_compute_normals(bool compute_normals) {
    _compute_normals = compute_normals;
}

double ocgd_BRepMesh_IncrementalMesh::get_linear_deflection() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get linear deflection - mesh is not initialized");
            return 0.0;
        }
        return _mesh->Parameters().Deflection;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting linear deflection - " + String(e.GetMessageString()));
        return 0.0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting linear deflection - " + String(e.what()));
        return 0.0;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_linear_deflection(double deflection) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set linear deflection - mesh is not initialized");
            return;
        }

        if (deflection <= 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Linear deflection must be positive, got: " + String::num(deflection));
            return;
        }

        if (!std::isfinite(deflection)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Linear deflection must be finite");
            return;
        }

        _mesh->ChangeParameters().Deflection = deflection;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting linear deflection - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting linear deflection - " + String(e.what()));
    }
}

double ocgd_BRepMesh_IncrementalMesh::get_angular_deflection() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get angular deflection - mesh is not initialized");
            return 0.0;
        }
        return _mesh->Parameters().Angle;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting angular deflection - " + String(e.GetMessageString()));
        return 0.0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting angular deflection - " + String(e.what()));
        return 0.0;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_angular_deflection(double deflection) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set angular deflection - mesh is not initialized");
            return;
        }

        if (deflection <= 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Angular deflection must be positive, got: " + String::num(deflection));
            return;
        }

        if (!std::isfinite(deflection)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Angular deflection must be finite");
            return;
        }

        _mesh->ChangeParameters().Angle = deflection;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting angular deflection - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting angular deflection - " + String(e.what()));
    }
}

bool ocgd_BRepMesh_IncrementalMesh::get_relative_deflection() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get relative deflection - mesh is not initialized");
            return false;
        }
        return _mesh->Parameters().Relative == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting relative deflection - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting relative deflection - " + String(e.what()));
        return false;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_relative_deflection(bool is_relative) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set relative deflection - mesh is not initialized");
            return;
        }
        _mesh->ChangeParameters().Relative = is_relative ? Standard_True : Standard_False;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting relative deflection - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting relative deflection - " + String(e.what()));
    }
}

bool ocgd_BRepMesh_IncrementalMesh::get_parallel_processing() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get parallel processing - mesh is not initialized");
            return false;
        }
        return _mesh->Parameters().InParallel == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting parallel processing - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting parallel processing - " + String(e.what()));
        return false;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_parallel_processing(bool is_parallel) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set parallel processing - mesh is not initialized");
            return;
        }
        _mesh->ChangeParameters().InParallel = is_parallel ? Standard_True : Standard_False;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting parallel processing - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting parallel processing - " + String(e.what()));
    }
}

bool ocgd_BRepMesh_IncrementalMesh::is_modified() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot check if modified - mesh is not initialized");
            return false;
        }
        return _mesh->IsModified() == Standard_True;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error checking modification status - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error checking modification status - " + String(e.what()));
        return false;
    }
}

int ocgd_BRepMesh_IncrementalMesh::get_status_flags() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get status flags - mesh is not initialized");
            return 0;
        }
        return _mesh->GetStatusFlags();
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting status flags - " + String(e.GetMessageString()));
        return 0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting status flags - " + String(e.what()));
        return 0;
    }
}

double ocgd_BRepMesh_IncrementalMesh::get_min_size() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get min size - mesh is not initialized");
            return 0.0;
        }
        return _mesh->Parameters().MinSize;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting min size - " + String(e.GetMessageString()));
        return 0.0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting min size - " + String(e.what()));
        return 0.0;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_min_size(double min_size) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set min size - mesh is not initialized");
            return;
        }

        if (min_size < 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Min size cannot be negative, got: " + String::num(min_size));
            return;
        }

        if (!std::isfinite(min_size)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Min size must be finite");
            return;
        }

        _mesh->ChangeParameters().MinSize = min_size;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting min size - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting min size - " + String(e.what()));
    }
}

double ocgd_BRepMesh_IncrementalMesh::get_interior_deflection() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get interior deflection - mesh is not initialized");
            return 0.0;
        }
        return _mesh->Parameters().DeflectionInterior;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting interior deflection - " + String(e.GetMessageString()));
        return 0.0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting interior deflection - " + String(e.what()));
        return 0.0;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_interior_deflection(double deflection) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set interior deflection - mesh is not initialized");
            return;
        }

        if (deflection < 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Interior deflection cannot be negative, got: " + String::num(deflection));
            return;
        }

        if (!std::isfinite(deflection)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Interior deflection must be finite");
            return;
        }

        _mesh->ChangeParameters().DeflectionInterior = deflection;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting interior deflection - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting interior deflection - " + String(e.what()));
    }
}

double ocgd_BRepMesh_IncrementalMesh::get_interior_angular_deflection() const {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot get interior angular deflection - mesh is not initialized");
            return 0.0;
        }
        return _mesh->Parameters().AngleInterior;
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting interior angular deflection - " + String(e.GetMessageString()));
        return 0.0;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error getting interior angular deflection - " + String(e.what()));
        return 0.0;
    }
}

void ocgd_BRepMesh_IncrementalMesh::set_interior_angular_deflection(double deflection) {
    try {
        if (_mesh == nullptr) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Cannot set interior angular deflection - mesh is not initialized");
            return;
        }

        if (deflection < 0.0) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Interior angular deflection cannot be negative, got: " + String::num(deflection));
            return;
        }

        if (!std::isfinite(deflection)) {
            UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Interior angular deflection must be finite");
            return;
        }

        _mesh->ChangeParameters().AngleInterior = deflection;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting interior angular deflection - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("BRepMesh_IncrementalMesh: Error setting interior angular deflection - " + String(e.what()));
    }
}

void ocgd_BRepMesh_IncrementalMesh::_bind_methods() {
    // Initialization methods
    ClassDB::bind_method(D_METHOD("init_with_shape", "shape", "linear_deflection", "is_relative", "angular_deflection", "is_in_parallel", "compute_normals"), 
                        &ocgd_BRepMesh_IncrementalMesh::init_with_shape, 
                        DEFVAL(false), DEFVAL(0.5), DEFVAL(false), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("perform"), &ocgd_BRepMesh_IncrementalMesh::perform);
    ClassDB::bind_method(D_METHOD("compute_normals"), &ocgd_BRepMesh_IncrementalMesh::compute_normals);
    
    // Linear deflection property
    ClassDB::bind_method(D_METHOD("get_linear_deflection"), &ocgd_BRepMesh_IncrementalMesh::get_linear_deflection);
    ClassDB::bind_method(D_METHOD("set_linear_deflection", "deflection"), &ocgd_BRepMesh_IncrementalMesh::set_linear_deflection);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::FLOAT, "linear_deflection"), "set_linear_deflection", "get_linear_deflection");
    
    // Angular deflection property
    ClassDB::bind_method(D_METHOD("get_angular_deflection"), &ocgd_BRepMesh_IncrementalMesh::get_angular_deflection);
    ClassDB::bind_method(D_METHOD("set_angular_deflection", "deflection"), &ocgd_BRepMesh_IncrementalMesh::set_angular_deflection);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::FLOAT, "angular_deflection"), "set_angular_deflection", "get_angular_deflection");
    
    // Relative deflection property
    ClassDB::bind_method(D_METHOD("get_relative_deflection"), &ocgd_BRepMesh_IncrementalMesh::get_relative_deflection);
    ClassDB::bind_method(D_METHOD("set_relative_deflection", "is_relative"), &ocgd_BRepMesh_IncrementalMesh::set_relative_deflection);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::BOOL, "relative_deflection"), "set_relative_deflection", "get_relative_deflection");
    
    // Parallel processing property
    ClassDB::bind_method(D_METHOD("get_parallel_processing"), &ocgd_BRepMesh_IncrementalMesh::get_parallel_processing);
    ClassDB::bind_method(D_METHOD("set_parallel_processing", "is_parallel"), &ocgd_BRepMesh_IncrementalMesh::set_parallel_processing);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::BOOL, "parallel_processing"), "set_parallel_processing", "get_parallel_processing");
    
    // Compute normals property
    ClassDB::bind_method(D_METHOD("get_compute_normals"), &ocgd_BRepMesh_IncrementalMesh::get_compute_normals);
    ClassDB::bind_method(D_METHOD("set_compute_normals", "compute_normals"), &ocgd_BRepMesh_IncrementalMesh::set_compute_normals);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::BOOL, "compute_normals"), "set_compute_normals", "get_compute_normals");
    
    // Min size property
    ClassDB::bind_method(D_METHOD("get_min_size"), &ocgd_BRepMesh_IncrementalMesh::get_min_size);
    ClassDB::bind_method(D_METHOD("set_min_size", "min_size"), &ocgd_BRepMesh_IncrementalMesh::set_min_size);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::FLOAT, "min_size"), "set_min_size", "get_min_size");
    
    // Interior deflection property
    ClassDB::bind_method(D_METHOD("get_interior_deflection"), &ocgd_BRepMesh_IncrementalMesh::get_interior_deflection);
    ClassDB::bind_method(D_METHOD("set_interior_deflection", "deflection"), &ocgd_BRepMesh_IncrementalMesh::set_interior_deflection);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::FLOAT, "interior_deflection"), "set_interior_deflection", "get_interior_deflection");
    
    // Interior angular deflection property
    ClassDB::bind_method(D_METHOD("get_interior_angular_deflection"), &ocgd_BRepMesh_IncrementalMesh::get_interior_angular_deflection);
    ClassDB::bind_method(D_METHOD("set_interior_angular_deflection", "deflection"), &ocgd_BRepMesh_IncrementalMesh::set_interior_angular_deflection);
    ClassDB::add_property("ocgd_BRepMesh_IncrementalMesh", PropertyInfo(Variant::FLOAT, "interior_angular_deflection"), "set_interior_angular_deflection", "get_interior_angular_deflection");
    
    // Status methods
    ClassDB::bind_method(D_METHOD("is_modified"), &ocgd_BRepMesh_IncrementalMesh::is_modified);
    ClassDB::bind_method(D_METHOD("get_status_flags"), &ocgd_BRepMesh_IncrementalMesh::get_status_flags);
    
    // Note: Static methods set_parallel_default and is_parallel_default 
    // cannot be bound through ClassDB, they would need to be exposed differently
}