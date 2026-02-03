/**
 * ocgd_PrimitiveShapes.cpp
 *
 * Godot GDExtension wrapper implementation for OpenCASCADE primitive shape creation.
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include "ocgd_PrimitiveShapes.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <opencascade/BRepPrimAPI_MakeBox.hxx>
#include <opencascade/BRepPrimAPI_MakeSphere.hxx>
#include <opencascade/BRepPrimAPI_MakeCylinder.hxx>
#include <opencascade/BRepPrimAPI_MakeCone.hxx>
#include <opencascade/BRepPrimAPI_MakeTorus.hxx>
#include <opencascade/BRepPrimAPI_MakeWedge.hxx>
#include <opencascade/BRepPrimAPI_MakePrism.hxx>
#include <opencascade/BRepPrimAPI_MakeRevol.hxx>
#include <opencascade/BRepBuilderAPI_MakeFace.hxx>
#include <opencascade/BRepBuilderAPI_MakeEdge.hxx>
#include <opencascade/BRepBuilderAPI_MakeWire.hxx>
#include <opencascade/BRepBuilderAPI_MakePolygon.hxx>
#include <opencascade/BRepFilletAPI_MakeFillet.hxx>
#include <opencascade/BRepFilletAPI_MakeChamfer.hxx>
#include <opencascade/gp_Pln.hxx>
#include <opencascade/gp_Circ.hxx>
#include <opencascade/gp_Ax2.hxx>
#include <opencascade/Precision.hxx>
#include <opencascade/Standard_Failure.hxx>



using namespace godot;

ocgd_PrimitiveShapes::ocgd_PrimitiveShapes() {
    // Constructor
}

ocgd_PrimitiveShapes::~ocgd_PrimitiveShapes() {
    // Destructor
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_box(double width, double height, double depth, const Vector3& center) {
    if (width <= 0 || height <= 0 || depth <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Box dimensions must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Pnt corner1(center.x - width/2, center.y - height/2, center.z - depth/2);
        gp_Pnt corner2(center.x + width/2, center.y + height/2, center.z + depth/2);

        BRepPrimAPI_MakeBox box_maker(corner1, corner2);
        box_maker.Build();

        if (box_maker.IsDone()) {
            return wrap_shape(box_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create box - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create box - " + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_box_from_corners(const Vector3& corner1, const Vector3& corner2) {
    try {
        // Validate that corners define a valid box
        if (corner1.x == corner2.x || corner1.y == corner2.y || corner1.z == corner2.z) {
            UtilityFunctions::printerr("PrimitiveShapes: Box corners must define a 3D volume");
            return Ref<ocgd_TopoDS_Shape>();
        }

        gp_Pnt gp_corner1(corner1.x, corner1.y, corner1.z);
        gp_Pnt gp_corner2(corner2.x, corner2.y, corner2.z);

        BRepPrimAPI_MakeBox box_maker(gp_corner1, gp_corner2);
        box_maker.Build();

        if (box_maker.IsDone()) {
            return wrap_shape(box_maker.Shape());
        } else {
            UtilityFunctions::printerr("PrimitiveShapes: Box creation failed");
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create box from corners - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create box from corners - " + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_sphere(double radius, const Vector3& center) {
    if (radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Sphere radius must be positive, got: " + String::num(radius));
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Pnt gp_center(center.x, center.y, center.z);

        BRepPrimAPI_MakeSphere sphere_maker(gp_center, radius);
        sphere_maker.Build();

        if (sphere_maker.IsDone()) {
            return wrap_shape(sphere_maker.Shape());
        } else {
            UtilityFunctions::printerr("PrimitiveShapes: Sphere creation failed");
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create sphere - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create sphere - " + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_sphere_sector(double radius, double angle_u, double angle_v, const Vector3& center) {
    if (radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Sphere radius must be positive, got: " + String::num(radius));
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (angle_u <= 0 || angle_u > 2 * M_PI || angle_v <= 0 || angle_v > M_PI) {
        UtilityFunctions::printerr("PrimitiveShapes: Invalid sphere sector angles - angle_u must be in (0, 2π], angle_v must be in (0, π]");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Pnt gp_center(center.x, center.y, center.z);

        BRepPrimAPI_MakeSphere sphere_maker(gp_center, radius, angle_u, angle_v);
        sphere_maker.Build();

        if (sphere_maker.IsDone()) {
            return wrap_shape(sphere_maker.Shape());
        } else {
            UtilityFunctions::printerr("PrimitiveShapes: Sphere sector creation failed");
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create sphere sector - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create sphere sector - " + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_cylinder(double radius, double height, const Vector3& center, const Vector3& axis) {
    if (radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Cylinder radius must be positive, got: " + String::num(radius));
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (height <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Cylinder height must be positive, got: " + String::num(height));
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (axis.length() < 1e-6) {
        UtilityFunctions::printerr("PrimitiveShapes: Cylinder axis vector must have non-zero length");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, axis);

        BRepPrimAPI_MakeCylinder cylinder_maker(coordinate_system, radius, height);
        cylinder_maker.Build();

        if (cylinder_maker.IsDone()) {
            return wrap_shape(cylinder_maker.Shape());
        } else {
            UtilityFunctions::printerr("PrimitiveShapes: Cylinder creation failed");
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create cylinder - " + String(e.GetMessageString()));
    } catch (const std::exception& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create cylinder - " + String(e.what()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_cylinder_sector(double radius, double height, double angle, const Vector3& center, const Vector3& axis) {
    if (radius <= 0 || height <= 0 || angle <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Cylinder parameters must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, axis);

        BRepPrimAPI_MakeCylinder cylinder_maker(coordinate_system, radius, height, angle);
        cylinder_maker.Build();

        if (cylinder_maker.IsDone()) {
            return wrap_shape(cylinder_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create cylinder sector");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_cone(double radius1, double radius2, double height, const Vector3& center, const Vector3& axis) {
    if (radius1 < 0 || radius2 < 0 || height <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Cone parameters invalid");
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (radius1 <= Precision::Confusion() && radius2 <= Precision::Confusion()) {
        UtilityFunctions::printerr("PrimitiveShapes: Cone must have at least one non-zero radius");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, axis);

        BRepPrimAPI_MakeCone cone_maker(coordinate_system, radius1, radius2, height);
        cone_maker.Build();

        if (cone_maker.IsDone()) {
            return wrap_shape(cone_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create cone");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_torus(double major_radius, double minor_radius, const Vector3& center, const Vector3& normal) {
    if (major_radius <= 0 || minor_radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Torus radii must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (minor_radius >= major_radius) {
        UtilityFunctions::printerr("PrimitiveShapes: Minor radius must be less than major radius");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, normal);

        BRepPrimAPI_MakeTorus torus_maker(coordinate_system, major_radius, minor_radius);
        torus_maker.Build();

        if (torus_maker.IsDone()) {
            return wrap_shape(torus_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create torus");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_torus_sector(double major_radius, double minor_radius, double angle1, double angle2, const Vector3& center, const Vector3& normal) {
    if (major_radius <= 0 || minor_radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Torus radii must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, normal);

        BRepPrimAPI_MakeTorus torus_maker(coordinate_system, major_radius, minor_radius, angle1, angle2);
        torus_maker.Build();

        if (torus_maker.IsDone()) {
            return wrap_shape(torus_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create torus sector");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_prism(const Ref<ocgd_TopoDS_Shape>& base_shape, const Vector3& direction) {
    if (base_shape.is_null() || base_shape->is_null()) {
        UtilityFunctions::printerr("PrimitiveShapes: Base shape for prism cannot be null");
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (direction.length() <= Precision::Confusion()) {
        UtilityFunctions::printerr("PrimitiveShapes: Prism direction vector cannot be zero");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& base = base_shape->get_occt_shape();
        gp_Vec extrusion_vector = vector3_to_vector(direction);

        BRepPrimAPI_MakePrism prism_maker(base, extrusion_vector);
        prism_maker.Build();

        if (prism_maker.IsDone()) {
            return wrap_shape(prism_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create prism");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_revolution(const Ref<ocgd_TopoDS_Shape>& profile_shape, const Vector3& axis_origin, const Vector3& axis_direction, double angle) {
    if (profile_shape.is_null() || profile_shape->is_null()) {
        UtilityFunctions::printerr("PrimitiveShapes: Profile shape for revolution cannot be null");
        return Ref<ocgd_TopoDS_Shape>();
    }

    if (axis_direction.length() <= Precision::Confusion()) {
        UtilityFunctions::printerr("PrimitiveShapes: Revolution axis direction cannot be zero");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        const TopoDS_Shape& profile = profile_shape->get_occt_shape();
        gp_Pnt axis_point = vector3_to_point(axis_origin);
        gp_Dir axis_dir = vector3_to_direction(axis_direction);
        gp_Ax1 revolution_axis(axis_point, axis_dir);

        BRepPrimAPI_MakeRevol revolution_maker(profile, revolution_axis, angle);
        revolution_maker.Build();

        if (revolution_maker.IsDone()) {
            return wrap_shape(revolution_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create revolution");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_compound(const Array& shapes) {
    if (shapes.size() == 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Cannot create compound from empty array");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (int i = 0; i < shapes.size(); i++) {
            Ref<ocgd_TopoDS_Shape> shape = shapes[i];
            if (shape.is_valid() && !shape->is_null()) {
                builder.Add(compound, shape->get_occt_shape());
            }
        }

        return wrap_shape(compound);
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create compound");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_rectangle(double width, double height, const Vector3& center, const Vector3& normal) {
    if (width <= 0 || height <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Rectangle dimensions must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, normal);

        // Create a plane at the center with the given normal
        gp_Pln plane(gp_Pnt(center.x, center.y, center.z), gp_Dir(normal.x, normal.y, normal.z));

        // Create face with bounds for the rectangle
        BRepBuilderAPI_MakeFace face_maker(plane, -width/2, width/2, -height/2, height/2);
        face_maker.Build();

        if (face_maker.IsDone()) {
            return wrap_shape(face_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create rectangle");
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_circle(double radius, const Vector3& center, const Vector3& normal) {
    if (radius <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Circle radius must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, normal);
        gp_Circ circle(coordinate_system, radius);

        // Create edge from circle
        BRepBuilderAPI_MakeEdge edge_maker(circle);
        if (!edge_maker.IsDone()) {
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create wire from edge
        BRepBuilderAPI_MakeWire wire_maker(edge_maker.Edge());
        if (!wire_maker.IsDone()) {
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create face from wire
        BRepBuilderAPI_MakeFace face_maker(wire_maker.Wire());
        face_maker.Build();

        if (face_maker.IsDone()) {
            return wrap_shape(face_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::print(String("Failed to create circle: ") + String(e.GetMessageString()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_ellipse(double radius_x, double radius_y, const Vector3& center, const Vector3& normal) {
    if (radius_x <= 0 || radius_y <= 0) {
        UtilityFunctions::printerr("PrimitiveShapes: Ellipse radii must be positive");
        return Ref<ocgd_TopoDS_Shape>();
    }

    try {
        gp_Ax2 coordinate_system = create_coordinate_system(center, normal);
        gp_Elips ellipse(coordinate_system, radius_x, radius_y);

        // Create edge from ellipse
        BRepBuilderAPI_MakeEdge edge_maker(ellipse);
        if (!edge_maker.IsDone()) {
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create wire from edge
        BRepBuilderAPI_MakeWire wire_maker(edge_maker.Edge());
        if (!wire_maker.IsDone()) {
            return Ref<ocgd_TopoDS_Shape>();
        }

        // Create face from wire
        BRepBuilderAPI_MakeFace face_maker(wire_maker.Wire());
        face_maker.Build();

        if (face_maker.IsDone()) {
            return wrap_shape(face_maker.Shape());
        }
    } catch (const Standard_Failure& e) {
        UtilityFunctions::print(String("Failed to create ellipse: ") + String(e.GetMessageString()));
    }

    return Ref<ocgd_TopoDS_Shape>();
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_cube(double size, const Vector3& center) {
    return create_box(size, size, size, center);
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_unit_cube() {
    return create_cube(1.0, Vector3(0, 0, 0));
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_unit_sphere() {
    return create_sphere(1.0, Vector3(0, 0, 0));
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::create_unit_cylinder() {
    return create_cylinder(1.0, 1.0, Vector3(0, 0, 0), Vector3(0, 0, 1));
}

gp_Pnt ocgd_PrimitiveShapes::vector3_to_point(const Vector3& v) const {
    return gp_Pnt(v.x, v.y, v.z);
}

gp_Vec ocgd_PrimitiveShapes::vector3_to_vector(const Vector3& v) const {
    return gp_Vec(v.x, v.y, v.z);
}

gp_Dir ocgd_PrimitiveShapes::vector3_to_direction(const Vector3& v) const {
    Vector3 normalized = v.normalized();
    return gp_Dir(normalized.x, normalized.y, normalized.z);
}

gp_Ax2 ocgd_PrimitiveShapes::create_coordinate_system(const Vector3& center, const Vector3& normal) const {
    gp_Pnt origin = vector3_to_point(center);
    gp_Dir z_dir = vector3_to_direction(normal);
    return gp_Ax2(origin, z_dir);
}

Ref<ocgd_TopoDS_Shape> ocgd_PrimitiveShapes::wrap_shape(const TopoDS_Shape& shape) const {
    if (shape.IsNull()) {
        return Ref<ocgd_TopoDS_Shape>();
    }

    Ref<ocgd_TopoDS_Shape> result = memnew(ocgd_TopoDS_Shape);
    result->set_occt_shape(shape);
    return result;
}

bool ocgd_PrimitiveShapes::validate_created_shape(const TopoDS_Shape& shape, const String& shape_type) const {
    if (shape.IsNull()) {
        UtilityFunctions::printerr("PrimitiveShapes: Failed to create " + shape_type + " - result is null");
        return false;
    }
    return true;
}

void ocgd_PrimitiveShapes::_bind_methods() {
    // Basic primitive creation methods
    ClassDB::bind_method(D_METHOD("create_box", "width", "height", "depth", "center"), &ocgd_PrimitiveShapes::create_box, DEFVAL(Vector3(0, 0, 0)));
    ClassDB::bind_method(D_METHOD("create_box_from_corners", "corner1", "corner2"), &ocgd_PrimitiveShapes::create_box_from_corners);

    ClassDB::bind_method(D_METHOD("create_sphere", "radius", "center"), &ocgd_PrimitiveShapes::create_sphere, DEFVAL(Vector3(0, 0, 0)));
    ClassDB::bind_method(D_METHOD("create_sphere_sector", "radius", "angle_u", "angle_v", "center"), &ocgd_PrimitiveShapes::create_sphere_sector, DEFVAL(Vector3(0, 0, 0)));

    ClassDB::bind_method(D_METHOD("create_cylinder", "radius", "height", "center", "axis"), &ocgd_PrimitiveShapes::create_cylinder, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));
    ClassDB::bind_method(D_METHOD("create_cylinder_sector", "radius", "height", "angle", "center", "axis"), &ocgd_PrimitiveShapes::create_cylinder_sector, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));

    ClassDB::bind_method(D_METHOD("create_cone", "radius1", "radius2", "height", "center", "axis"), &ocgd_PrimitiveShapes::create_cone, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));

    ClassDB::bind_method(D_METHOD("create_torus", "major_radius", "minor_radius", "center", "normal"), &ocgd_PrimitiveShapes::create_torus, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));
    ClassDB::bind_method(D_METHOD("create_torus_sector", "major_radius", "minor_radius", "angle1", "angle2", "center", "normal"), &ocgd_PrimitiveShapes::create_torus_sector, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));

    // Advanced creation methods
    ClassDB::bind_method(D_METHOD("create_prism", "base_shape", "direction"), &ocgd_PrimitiveShapes::create_prism);
    ClassDB::bind_method(D_METHOD("create_revolution", "profile_shape", "axis_origin", "axis_direction", "angle"), &ocgd_PrimitiveShapes::create_revolution, DEFVAL(6.28318530718));
    ClassDB::bind_method(D_METHOD("create_compound", "shapes"), &ocgd_PrimitiveShapes::create_compound);

    // 2D primitives
    ClassDB::bind_method(D_METHOD("create_rectangle", "width", "height", "center", "normal"), &ocgd_PrimitiveShapes::create_rectangle, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));
    ClassDB::bind_method(D_METHOD("create_circle", "radius", "center", "normal"), &ocgd_PrimitiveShapes::create_circle, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));
    ClassDB::bind_method(D_METHOD("create_ellipse", "radius_x", "radius_y", "center", "normal"), &ocgd_PrimitiveShapes::create_ellipse, DEFVAL(Vector3(0, 0, 0)), DEFVAL(Vector3(0, 0, 1)));

    // Quick creation methods
    ClassDB::bind_method(D_METHOD("create_cube", "size", "center"), &ocgd_PrimitiveShapes::create_cube, DEFVAL(Vector3(0, 0, 0)));
    ClassDB::bind_method(D_METHOD("create_unit_cube"), &ocgd_PrimitiveShapes::create_unit_cube);
    ClassDB::bind_method(D_METHOD("create_unit_sphere"), &ocgd_PrimitiveShapes::create_unit_sphere);
    ClassDB::bind_method(D_METHOD("create_unit_cylinder"), &ocgd_PrimitiveShapes::create_unit_cylinder);
}
