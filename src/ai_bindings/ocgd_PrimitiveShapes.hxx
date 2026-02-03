#ifndef _ocgd_PrimitiveShapes_HeaderFile
#define _ocgd_PrimitiveShapes_HeaderFile

/**
 * ocgd_PrimitiveShapes.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE primitive shape creation.
 *
 * This class provides methods to create basic geometric primitives including:
 * - Boxes (rectangular solids)
 * - Spheres and ellipsoids
 * - Cylinders and cones
 * - Torus shapes
 * - Wedges and pyramids
 * - Prisms and swept shapes
 * - Revolution shapes
 *
 * These primitives serve as building blocks for more complex CAD models
 * and can be combined using Boolean operations to create sophisticated
 * geometries. All shapes are created as valid OpenCASCADE solids with
 * proper topology.
 *
 * Features:
 * - Standard geometric primitives with customizable parameters
 * - Positioning and orientation control
 * - Validation and error checking
 * - Integration with the existing shape ecosystem
 *
 * Original OCCT headers: <opencascade/BRepPrimAPI_MakeBox.hxx>,
 *                       <opencascade/BRepPrimAPI_MakeSphere.hxx>,
 *                       <opencascade/BRepPrimAPI_MakeCylinder.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/gp_Pnt.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Dir.hxx>
#include <opencascade/gp_Ax1.hxx>
#include <opencascade/gp_Ax2.hxx>
#include <opencascade/gp_Pln.hxx>
#include <opencascade/gp_Circ.hxx>
#include <opencascade/gp_Elips.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_PrimitiveShapes
 *
 * Utility class for creating basic geometric primitive shapes in OpenCASCADE.
 *
 * This class provides static-like methods to create common 3D primitives
 * with intuitive parameters. All shapes are created centered at the origin
 * by default, but can be positioned and oriented as needed.
 */
class ocgd_PrimitiveShapes : public RefCounted {
    GDCLASS(ocgd_PrimitiveShapes, RefCounted);

protected:
    static void _bind_methods();

public:
    //! Default constructor
    ocgd_PrimitiveShapes();

    //! Destructor
    virtual ~ocgd_PrimitiveShapes();

    // Basic Primitive Creation Methods

    //! Create a rectangular box (cuboid)
    //! @param width size along X axis
    //! @param height size along Y axis  
    //! @param depth size along Z axis
    //! @param center center position of the box
    //! @return the created box shape
    Ref<ocgd_TopoDS_Shape> create_box(double width, double height, double depth, 
                                     const Vector3& center = Vector3(0, 0, 0));

    //! Create a box from two corner points
    //! @param corner1 first corner of the box
    //! @param corner2 opposite corner of the box
    //! @return the created box shape
    Ref<ocgd_TopoDS_Shape> create_box_from_corners(const Vector3& corner1, const Vector3& corner2);

    //! Create a sphere
    //! @param radius radius of the sphere
    //! @param center center position of the sphere
    //! @return the created sphere shape
    Ref<ocgd_TopoDS_Shape> create_sphere(double radius, const Vector3& center = Vector3(0, 0, 0));

    //! Create a partial sphere (sector)
    //! @param radius radius of the sphere
    //! @param angle_u U parameter angle in radians (0 to 2*PI)
    //! @param angle_v V parameter angle in radians (0 to PI)
    //! @param center center position of the sphere
    //! @return the created sphere sector shape
    Ref<ocgd_TopoDS_Shape> create_sphere_sector(double radius, double angle_u, double angle_v,
                                               const Vector3& center = Vector3(0, 0, 0));

    //! Create a cylinder
    //! @param radius radius of the cylinder
    //! @param height height of the cylinder
    //! @param center center position of the cylinder base
    //! @param axis direction vector for the cylinder axis (default: Z-up)
    //! @return the created cylinder shape
    Ref<ocgd_TopoDS_Shape> create_cylinder(double radius, double height,
                                          const Vector3& center = Vector3(0, 0, 0),
                                          const Vector3& axis = Vector3(0, 0, 1));

    //! Create a partial cylinder (sector)
    //! @param radius radius of the cylinder
    //! @param height height of the cylinder
    //! @param angle angle in radians for the cylinder sector
    //! @param center center position of the cylinder base
    //! @param axis direction vector for the cylinder axis
    //! @return the created cylinder sector shape
    Ref<ocgd_TopoDS_Shape> create_cylinder_sector(double radius, double height, double angle,
                                                  const Vector3& center = Vector3(0, 0, 0),
                                                  const Vector3& axis = Vector3(0, 0, 1));

    //! Create a cone
    //! @param radius1 radius at the base
    //! @param radius2 radius at the top
    //! @param height height of the cone
    //! @param center center position of the cone base
    //! @param axis direction vector for the cone axis
    //! @return the created cone shape
    Ref<ocgd_TopoDS_Shape> create_cone(double radius1, double radius2, double height,
                                      const Vector3& center = Vector3(0, 0, 0),
                                      const Vector3& axis = Vector3(0, 0, 1));

    //! Create a torus
    //! @param major_radius major radius (distance from center to tube center)
    //! @param minor_radius minor radius (tube radius)
    //! @param center center position of the torus
    //! @param normal normal vector for the torus plane
    //! @return the created torus shape
    Ref<ocgd_TopoDS_Shape> create_torus(double major_radius, double minor_radius,
                                       const Vector3& center = Vector3(0, 0, 0),
                                       const Vector3& normal = Vector3(0, 0, 1));

    //! Create a partial torus (sector)
    //! @param major_radius major radius
    //! @param minor_radius minor radius
    //! @param angle1 start angle in radians
    //! @param angle2 end angle in radians
    //! @param center center position of the torus
    //! @param normal normal vector for the torus plane
    //! @return the created torus sector shape
    Ref<ocgd_TopoDS_Shape> create_torus_sector(double major_radius, double minor_radius,
                                              double angle1, double angle2,
                                              const Vector3& center = Vector3(0, 0, 0),
                                              const Vector3& normal = Vector3(0, 0, 1));

    // Advanced Primitive Creation

    //! Create a wedge (triangular prism)
    //! @param dx size along X axis
    //! @param dy size along Y axis
    //! @param dz size along Z axis
    //! @param xmin minimum X coordinate for the slanted face
    //! @param zmin minimum Z coordinate for the slanted face
    //! @param xmax maximum X coordinate for the slanted face
    //! @param zmax maximum Z coordinate for the slanted face
    //! @param center center position of the wedge
    //! @return the created wedge shape
    Ref<ocgd_TopoDS_Shape> create_wedge(double dx, double dy, double dz,
                                       double xmin, double zmin, double xmax, double zmax,
                                       const Vector3& center = Vector3(0, 0, 0));

    //! Create a prism by extruding a base shape
    //! @param base_shape the 2D shape to extrude (face or wire)
    //! @param direction extrusion direction and distance
    //! @return the created prism shape
    Ref<ocgd_TopoDS_Shape> create_prism(const Ref<ocgd_TopoDS_Shape>& base_shape, 
                                       const Vector3& direction);

    //! Create a revolution shape by rotating a profile around an axis
    //! @param profile_shape the shape to revolve (edge, wire, or face)
    //! @param axis_origin point on the revolution axis
    //! @param axis_direction direction of the revolution axis
    //! @param angle revolution angle in radians (default: full revolution)
    //! @return the created revolution shape
    Ref<ocgd_TopoDS_Shape> create_revolution(const Ref<ocgd_TopoDS_Shape>& profile_shape,
                                            const Vector3& axis_origin,
                                            const Vector3& axis_direction,
                                            double angle = 6.28318530718); // 2*PI

    // Compound Shape Creation

    //! Create a compound shape from multiple shapes
    //! @param shapes array of shapes to combine into a compound
    //! @return the created compound shape
    Ref<ocgd_TopoDS_Shape> create_compound(const Array& shapes);

    // 2D Primitive Creation (for use with prisms, etc.)

    //! Create a rectangular face (2D rectangle)
    //! @param width width of the rectangle
    //! @param height height of the rectangle
    //! @param center center position of the rectangle
    //! @param normal normal vector for the rectangle plane
    //! @return the created rectangular face
    Ref<ocgd_TopoDS_Shape> create_rectangle(double width, double height,
                                           const Vector3& center = Vector3(0, 0, 0),
                                           const Vector3& normal = Vector3(0, 0, 1));

    //! Create a circular face (2D circle)
    //! @param radius radius of the circle
    //! @param center center position of the circle
    //! @param normal normal vector for the circle plane
    //! @return the created circular face
    Ref<ocgd_TopoDS_Shape> create_circle(double radius,
                                        const Vector3& center = Vector3(0, 0, 0),
                                        const Vector3& normal = Vector3(0, 0, 1));

    //! Create an elliptical face (2D ellipse)
    //! @param radius_x radius along the X axis
    //! @param radius_y radius along the Y axis
    //! @param center center position of the ellipse
    //! @param normal normal vector for the ellipse plane
    //! @return the created elliptical face
    Ref<ocgd_TopoDS_Shape> create_ellipse(double radius_x, double radius_y,
                                         const Vector3& center = Vector3(0, 0, 0),
                                         const Vector3& normal = Vector3(0, 0, 1));

    //! Create a polygonal face from an array of points
    //! @param points array of Vector3 points defining the polygon
    //! @param closed whether to close the polygon (connect last to first point)
    //! @return the created polygonal face (or wire if not closed)
    Ref<ocgd_TopoDS_Shape> create_polygon(const Array& points, bool closed = true);

    // Validation and Utility Methods

    //! Validate that primitive parameters are valid
    //! @param primitive_type name of the primitive type
    //! @param parameters dictionary with primitive parameters
    //! @return Dictionary with validation results
    Dictionary validate_primitive_parameters(const String& primitive_type, 
                                            const Dictionary& parameters);

    //! Get recommended mesh parameters for a primitive
    //! @param shape the primitive shape
    //! @param quality_level quality level (0.0 = coarse, 1.0 = fine)
    //! @return Dictionary with recommended mesh parameters
    Dictionary get_recommended_mesh_params(const Ref<ocgd_TopoDS_Shape>& shape, 
                                          double quality_level = 0.5);

    //! Get primitive shape information
    //! @param shape the shape to analyze
    //! @return Dictionary with shape characteristics
    Dictionary get_primitive_info(const Ref<ocgd_TopoDS_Shape>& shape);

    // Quick Creation Methods with Common Defaults

    //! Quick cube creation (equal sides)
    //! @param size side length of the cube
    //! @param center center position
    //! @return the created cube shape
    Ref<ocgd_TopoDS_Shape> create_cube(double size, const Vector3& center = Vector3(0, 0, 0));

    //! Quick unit shapes (size = 1.0, centered at origin)
    Ref<ocgd_TopoDS_Shape> create_unit_cube();
    Ref<ocgd_TopoDS_Shape> create_unit_sphere();
    Ref<ocgd_TopoDS_Shape> create_unit_cylinder();

private:
    //! Internal helper to convert Vector3 to gp_Pnt
    gp_Pnt vector3_to_point(const Vector3& v) const;

    //! Internal helper to convert Vector3 to gp_Vec
    gp_Vec vector3_to_vector(const Vector3& v) const;

    //! Internal helper to convert Vector3 to gp_Dir
    gp_Dir vector3_to_direction(const Vector3& v) const;

    //! Internal helper to create coordinate system from center and normal
    gp_Ax2 create_coordinate_system(const Vector3& center, const Vector3& normal) const;

    //! Internal helper to wrap TopoDS_Shape in Godot wrapper
    Ref<ocgd_TopoDS_Shape> wrap_shape(const TopoDS_Shape& shape) const;

    //! Internal helper to validate shape creation result
    bool validate_created_shape(const TopoDS_Shape& shape, const String& shape_type) const;
};

#endif // _ocgd_PrimitiveShapes_HeaderFile