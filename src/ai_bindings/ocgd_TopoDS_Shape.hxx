#ifndef _ocgd_TopoDS_Shape_HeaderFile
#define _ocgd_TopoDS_Shape_HeaderFile

/**
 * ocgd_TopoDS_Shape.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE TopoDS_Shape class.
 *
 * Describes a shape which
 * - references an underlying shape with the potential
 * to be given a location and an orientation
 * - has a location for the underlying shape, giving its
 * placement in the local coordinate system
 * - has an orientation for the underlying shape, in
 * terms of its geometry (as opposed to orientation in
 * relation to other shapes).
 * Note: A Shape is empty if it references an underlying
 * shape which has an empty list of shapes.
 *
 * Original OCCT header: <opencascade/TopoDS_Shape.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>
#include <opencascade/TopAbs_Orientation.hxx>
#include <opencascade/TopLoc_Location.hxx>
#include <opencascade/Standard_Handle.hxx>
#include <opencascade/gp_Vec.hxx>
#include <opencascade/gp_Trsf.hxx>

using namespace godot;

/**
 * ocgd_TopoDS_Shape
 *
 * Wrapper class to expose the OpenCASCADE TopoDS_Shape to Godot.
 *
 * Describes a shape which references an underlying shape with the potential
 * to be given a location and an orientation. Has a location for the underlying 
 * shape, giving its placement in the local coordinate system. Has an orientation 
 * for the underlying shape, in terms of its geometry (as opposed to orientation 
 * in relation to other shapes).
 *
 * Note: A Shape is empty if it references an underlying shape which has an 
 * empty list of shapes.
 */
class ocgd_TopoDS_Shape : public RefCounted {
    GDCLASS(ocgd_TopoDS_Shape, RefCounted);

protected:
    static void _bind_methods();

private:
    TopoDS_Shape _shape;

public:
    //! Creates a NULL Shape referring to nothing.
    ocgd_TopoDS_Shape();

    //! Creates a shape from an existing OpenCASCADE TopoDS_Shape
    ocgd_TopoDS_Shape(const TopoDS_Shape& shape);

    //! Destructor.
    virtual ~ocgd_TopoDS_Shape();

    //! Returns true if this shape is null. In other words, it
    //! references no underlying shape with the potential to
    //! be given a location and an orientation.
    bool is_null() const;

    //! Destroys the reference to the underlying shape
    //! stored in this shape. As a result, this shape becomes null.
    void nullify();

    //! Returns the type of the shape, returns TopAbs_SHAPE if the shape is null.
    int shape_type() const;

    //! Returns the orientation of the shape.
    int orientation() const;

    //! Sets the orientation of the shape.
    void set_orientation(int orient);

    //! Returns the shape local coordinate system location.
    Vector3 location() const;

    //! Returns True if two shapes are the same, i.e. if they share the same TShape
    //! with the same Locations. Orientations may differ.
    bool is_same(const Ref<ocgd_TopoDS_Shape>& other) const;

    //! Returns True if two shapes are equal, i.e. if they share the same TShape
    //! with the same Locations and Orientations.
    bool is_equal(const Ref<ocgd_TopoDS_Shape>& other) const;

    //! Returns True if the shape is not null and has the correct type.
    bool is_valid() const;

    //! Creates a copy of this shape with the same TShape and Location but with
    //! a different Orientation.
    Ref<ocgd_TopoDS_Shape> oriented(int orient) const;

    //! Creates a copy of this shape with the same TShape and Orientation but
    //! with a different Location.
    Ref<ocgd_TopoDS_Shape> located(const Vector3& loc) const;

    //! Returns a shape similar to this one with the local coordinate system
    //! set to the identity transformation.
    Ref<ocgd_TopoDS_Shape> moved(const Vector3& loc) const;

    //! Returns the complement of the orientation, using the orientation reversal map.
    Ref<ocgd_TopoDS_Shape> reversed() const;

    //! Returns the complement of the orientation
    Ref<ocgd_TopoDS_Shape> complemented() const;

    //! Creates a new empty copy of this shape.
    Ref<ocgd_TopoDS_Shape> empty_copy() const;

    //! Returns a hash code value for this shape.
    int hash_code() const;

    // Shape type enumeration constants for GDScript
    enum ShapeType {
        COMPOUND = TopAbs_COMPOUND,
        COMPSOLID = TopAbs_COMPSOLID, 
        SOLID = TopAbs_SOLID,
        SHELL = TopAbs_SHELL,
        FACE = TopAbs_FACE,
        WIRE = TopAbs_WIRE,
        EDGE = TopAbs_EDGE,
        VERTEX = TopAbs_VERTEX,
        SHAPE = TopAbs_SHAPE
    };

    // Orientation enumeration constants for GDScript
    enum Orientation {
        FORWARD = TopAbs_FORWARD,
        REVERSED = TopAbs_REVERSED,
        INTERNAL = TopAbs_INTERNAL,
        EXTERNAL = TopAbs_EXTERNAL
    };

    // Internal access to the wrapped OpenCASCADE object
    const TopoDS_Shape& get_occt_shape() const { return _shape; }
    TopoDS_Shape& get_occt_shape() { return _shape; }
    void set_occt_shape(const TopoDS_Shape& shape) { _shape = shape; }
};

VARIANT_ENUM_CAST(ocgd_TopoDS_Shape::ShapeType);
VARIANT_ENUM_CAST(ocgd_TopoDS_Shape::Orientation);

#endif // _ocgd_TopoDS_Shape_HeaderFile