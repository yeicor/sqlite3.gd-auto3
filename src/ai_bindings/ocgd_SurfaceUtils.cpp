/**
 * ocgd_SurfaceUtils.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE surface utility functionality.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_SurfaceUtils.hxx"
#include "ocgd_EnhancedNormals.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/BRepAdaptor_Surface.hxx>
#include <opencascade/BRepLProp_SLProps.hxx>
#include <opencascade/BRepTools.hxx>
#include <opencascade/TopoDS.hxx>
#include <opencascade/TopoDS_Face.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/TopAbs.hxx>
#include <opencascade/GeomAbs_SurfaceType.hxx>
#include <opencascade/Standard_Failure.hxx>

using namespace godot;

ocgd_SurfaceUtils::ocgd_SurfaceUtils() : _tolerance(1e-6) {
}

ocgd_SurfaceUtils::~ocgd_SurfaceUtils() {
}

Vector3 ocgd_SurfaceUtils::compute_surface_normal(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const {
    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute normal - face reference is null");
            return Vector3(0, 0, 1); // Default up vector
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute normal - face is null");
            return Vector3(0, 0, 1);
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        BRepLProp_SLProps props(surface, u, v, 1, _tolerance);

        if (props.IsNormalDefined()) {
            gp_Dir normal = props.Normal();
            Vector3 result = gp_dir_to_vector3(normal);
            
            // Triangle winding order already handles orientation, only flip if explicitly requested
            if (ocgd_EnhancedNormals::get_flip_normals_for_reversed() && occt_face.Orientation() == TopAbs_REVERSED) {
                result = -result;
            }
            
            return result;
        } else {
            UtilityFunctions::printerr("SurfaceUtils: Normal not defined at UV coordinates (" +
                                     String::num(u) + ", " + String::num(v) + ")");
            return Vector3(0, 0, 1);
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface normal - " + String(e.GetMessageString()));
        return Vector3(0, 0, 1);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface normal - " + String(e.what()));
        return Vector3(0, 0, 1);
    }
}

Dictionary ocgd_SurfaceUtils::compute_surface_normal_axis(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const {
    Dictionary result;

    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute normal axis - face reference is null");
            return result;
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute normal axis - face is null");
            return result;
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        BRepLProp_SLProps props(surface, u, v, 1, _tolerance);

        if (props.IsNormalDefined()) {
            gp_Pnt point = props.Value();
            gp_Dir normal = props.Normal();
            Vector3 normal_vec = gp_dir_to_vector3(normal);
            
            // Triangle winding order already handles orientation, only flip if explicitly requested
            if (ocgd_EnhancedNormals::get_flip_normals_for_reversed() && occt_face.Orientation() == TopAbs_REVERSED) {
                normal_vec = -normal_vec;
            }
            
            result["point"] = gp_pnt_to_vector3(point);
            result["direction"] = normal_vec;
            result["valid"] = true;
        } else {
            UtilityFunctions::printerr("SurfaceUtils: Normal not defined at UV coordinates (" +
                                     String::num(u) + ", " + String::num(v) + ")");
            result["valid"] = false;
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface normal axis - " + String(e.GetMessageString()));
        result["valid"] = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface normal axis - " + String(e.what()));
        result["valid"] = false;
    }

    return result;
}

Dictionary ocgd_SurfaceUtils::get_uv_bounds(const Ref<ocgd_TopoDS_Shape>& face) const {
    Dictionary result;

    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot get UV bounds - face reference is null");
            return result;
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot get UV bounds - face is null");
            return result;
        }

        TopoDS_Face occt_face = shape_to_face(face);

        Standard_Real u_min, u_max, v_min, v_max;
        BRepTools::UVBounds(occt_face, u_min, u_max, v_min, v_max);

        result["u_min"] = u_min;
        result["u_max"] = u_max;
        result["v_min"] = v_min;
        result["v_max"] = v_max;
        result["valid"] = true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to get UV bounds - " + String(e.GetMessageString()));
        result["valid"] = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to get UV bounds - " + String(e.what()));
        result["valid"] = false;
    }

    return result;
}

Vector3 ocgd_SurfaceUtils::compute_surface_point(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const {
    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute point - face reference is null");
            return Vector3(0, 0, 0);
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute point - face is null");
            return Vector3(0, 0, 0);
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        BRepLProp_SLProps props(surface, u, v, 0, _tolerance);
        gp_Pnt point = props.Value();

        return gp_pnt_to_vector3(point);

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface point - " + String(e.GetMessageString()));
        return Vector3(0, 0, 0);
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface point - " + String(e.what()));
        return Vector3(0, 0, 0);
    }
}

Dictionary ocgd_SurfaceUtils::compute_surface_curvature(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const {
    Dictionary result;

    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute curvature - face reference is null");
            return result;
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot compute curvature - face is null");
            return result;
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        BRepLProp_SLProps props(surface, u, v, 2, _tolerance);

        result["normal_defined"] = props.IsNormalDefined();
        result["curvature_defined"] = props.IsCurvatureDefined();

        if (props.IsNormalDefined()) {
            gp_Dir normal = props.Normal();
            Vector3 normal_vec = gp_dir_to_vector3(normal);
            
            // Triangle winding order already handles orientation, only flip if explicitly requested
            if (ocgd_EnhancedNormals::get_flip_normals_for_reversed() && occt_face.Orientation() == TopAbs_REVERSED) {
                normal_vec = -normal_vec;
            }
            
            result["normal"] = normal_vec;
        }

        if (props.IsCurvatureDefined()) {
            result["gaussian_curvature"] = props.GaussianCurvature();
            result["mean_curvature"] = props.MeanCurvature();
            result["min_curvature"] = props.MinCurvature();
            result["max_curvature"] = props.MaxCurvature();
        }

        result["valid"] = true;

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface curvature - " + String(e.GetMessageString()));
        result["valid"] = false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to compute surface curvature - " + String(e.what()));
        result["valid"] = false;
    }

    return result;
}

bool ocgd_SurfaceUtils::is_normal_defined(const Ref<ocgd_TopoDS_Shape>& face, double u, double v) const {
    try {
        if (face.is_null() || face->is_null()) {
            return false;
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        BRepLProp_SLProps props(surface, u, v, 1, _tolerance);
        return props.IsNormalDefined();

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to check normal definition - " + String(e.GetMessageString()));
        return false;
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to check normal definition - " + String(e.what()));
        return false;
    }
}

Array ocgd_SurfaceUtils::sample_surface_normals(const Ref<ocgd_TopoDS_Shape>& face, const Array& u_samples, const Array& v_samples) const {
    Array result;

    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot sample normals - face reference is null");
            return result;
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot sample normals - face is null");
            return result;
        }

        if (u_samples.size() != v_samples.size()) {
            UtilityFunctions::printerr("SurfaceUtils: UV sample arrays must have the same size");
            return result;
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);

        for (int i = 0; i < u_samples.size(); i++) {
            double u = u_samples[i];
            double v = v_samples[i];

            BRepLProp_SLProps props(surface, u, v, 1, _tolerance);

            if (props.IsNormalDefined()) {
                gp_Dir normal = props.Normal();
                Vector3 normal_vec = gp_dir_to_vector3(normal);
                
                // Triangle winding order already handles orientation, only flip if explicitly requested
                if (ocgd_EnhancedNormals::get_flip_normals_for_reversed() && occt_face.Orientation() == TopAbs_REVERSED) {
                    normal_vec = -normal_vec;
                }
                
                result.append(normal_vec);
            } else {
                result.append(Vector3(0, 0, 1)); // Default normal
            }
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to sample surface normals - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to sample surface normals - " + String(e.what()));
    }

    return result;
}

String ocgd_SurfaceUtils::get_surface_type(const Ref<ocgd_TopoDS_Shape>& face) const {
    try {
        if (face.is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot get surface type - face reference is null");
            return "unknown";
        }

        if (face->is_null()) {
            UtilityFunctions::printerr("SurfaceUtils: Cannot get surface type - face is null");
            return "unknown";
        }

        TopoDS_Face occt_face = shape_to_face(face);
        BRepAdaptor_Surface surface(occt_face);
        GeomAbs_SurfaceType surface_type = surface.GetType();

        switch (surface_type) {
            case GeomAbs_Plane:
                return "plane";
            case GeomAbs_Cylinder:
                return "cylinder";
            case GeomAbs_Cone:
                return "cone";
            case GeomAbs_Sphere:
                return "sphere";
            case GeomAbs_Torus:
                return "torus";
            case GeomAbs_BezierSurface:
                return "bezier";
            case GeomAbs_BSplineSurface:
                return "bspline";
            case GeomAbs_SurfaceOfRevolution:
                return "revolution";
            case GeomAbs_SurfaceOfExtrusion:
                return "extrusion";
            default:
                return "other";
        }

    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to get surface type - " + String(e.GetMessageString()));
        return "unknown";
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("SurfaceUtils: Failed to get surface type - " + String(e.what()));
        return "unknown";
    }
}

void ocgd_SurfaceUtils::set_tolerance(double tolerance) {
    if (tolerance > 0.0) {
        _tolerance = tolerance;
    } else {
        UtilityFunctions::printerr("SurfaceUtils: Tolerance must be positive, using default");
        _tolerance = 1e-6;
    }
}

double ocgd_SurfaceUtils::get_tolerance() const {
    return _tolerance;
}

void ocgd_SurfaceUtils::set_flip_normals_for_reversed(bool flip_for_reversed) {
    ocgd_EnhancedNormals::set_flip_normals_for_reversed(flip_for_reversed);
}

bool ocgd_SurfaceUtils::get_flip_normals_for_reversed() {
    return ocgd_EnhancedNormals::get_flip_normals_for_reversed();
}

TopoDS_Face ocgd_SurfaceUtils::shape_to_face(const Ref<ocgd_TopoDS_Shape>& shape) const {
    const TopoDS_Shape& occt_shape = shape->get_occt_shape();
    
    if (occt_shape.ShapeType() != TopAbs_FACE) {
        throw Standard_Failure("Shape is not a face");
    }
    
    return TopoDS::Face(occt_shape);
}

Vector3 ocgd_SurfaceUtils::gp_pnt_to_vector3(const gp_Pnt& point) const {
    return Vector3(point.X(), point.Y(), point.Z());
}

Vector3 ocgd_SurfaceUtils::gp_dir_to_vector3(const gp_Dir& direction) const {
    return Vector3(direction.X(), direction.Y(), direction.Z());
}

Vector3 ocgd_SurfaceUtils::gp_vec_to_vector3(const gp_Vec& vector) const {
    return Vector3(vector.X(), vector.Y(), vector.Z());
}

void ocgd_SurfaceUtils::_bind_methods() {
    // Surface normal computation methods
    ClassDB::bind_method(D_METHOD("compute_surface_normal", "face", "u", "v"), &ocgd_SurfaceUtils::compute_surface_normal);
    ClassDB::bind_method(D_METHOD("compute_surface_normal_axis", "face", "u", "v"), &ocgd_SurfaceUtils::compute_surface_normal_axis);
    ClassDB::bind_method(D_METHOD("is_normal_defined", "face", "u", "v"), &ocgd_SurfaceUtils::is_normal_defined);
    ClassDB::bind_method(D_METHOD("sample_surface_normals", "face", "u_samples", "v_samples"), &ocgd_SurfaceUtils::sample_surface_normals);

    // Surface property methods
    ClassDB::bind_method(D_METHOD("get_uv_bounds", "face"), &ocgd_SurfaceUtils::get_uv_bounds);
    ClassDB::bind_method(D_METHOD("compute_surface_point", "face", "u", "v"), &ocgd_SurfaceUtils::compute_surface_point);
    ClassDB::bind_method(D_METHOD("compute_surface_curvature", "face", "u", "v"), &ocgd_SurfaceUtils::compute_surface_curvature);
    ClassDB::bind_method(D_METHOD("get_surface_type", "face"), &ocgd_SurfaceUtils::get_surface_type);

    // Configuration methods
    ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &ocgd_SurfaceUtils::set_tolerance);
    ClassDB::bind_method(D_METHOD("get_tolerance"), &ocgd_SurfaceUtils::get_tolerance);
    
    // Static normal orientation control
    ClassDB::bind_static_method("ocgd_SurfaceUtils", D_METHOD("set_flip_normals_for_reversed", "flip_for_reversed"), &ocgd_SurfaceUtils::set_flip_normals_for_reversed);
    ClassDB::bind_static_method("ocgd_SurfaceUtils", D_METHOD("get_flip_normals_for_reversed"), &ocgd_SurfaceUtils::get_flip_normals_for_reversed);
}
