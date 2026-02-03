#ifndef _ocgd_TopExp_Explorer_HeaderFile
#define _ocgd_TopExp_Explorer_HeaderFile

/**
 * ocgd_TopExp_Explorer.hxx
 *
 * Godot GDExtension wrapper for OpenCASCADE TopExp_Explorer class.
 *
 * An Explorer is a Tool to visit a Topological Data Structure from the TopoDS package.
 *
 * An Explorer is built with:
 * - The Shape to explore.
 * - The type of Shapes to find: e.g VERTEX, EDGE. This type cannot be SHAPE.
 * - The type of Shapes to avoid. e.g SHELL, EDGE. By default this type is SHAPE
 *   which means no restriction on the exploration.
 *
 * The Explorer visits all the structure to find shapes of the requested type
 * which are not contained in the type to avoid.
 *
 * Example to find all the Faces in the Shape S:
 * TopExp_Explorer Ex;
 * for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) {
 *   ProcessFace(Ex.Current());
 * }
 *
 * Original OCCT header: <opencascade/TopExp_Explorer.hxx>
 *
 * This file is part of OpenCASCADE.gd bindings.
 */

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/string.hpp>

#include <opencascade/TopExp_Explorer.hxx>
#include <opencascade/TopoDS_Shape.hxx>
#include <opencascade/TopAbs_ShapeEnum.hxx>

#include "ocgd_TopoDS_Shape.hxx"

using namespace godot;

/**
 * ocgd_TopExp_Explorer
 *
 * Wrapper class to expose the OpenCASCADE TopExp_Explorer to Godot.
 *
 * An Explorer is a Tool to visit a Topological Data Structure from the TopoDS package.
 * The Explorer visits all the structure to find shapes of the requested type
 * which are not contained in the type to avoid.
 */
class ocgd_TopExp_Explorer : public RefCounted {
    GDCLASS(ocgd_TopExp_Explorer, RefCounted);

protected:
    static void _bind_methods();

private:
    TopExp_Explorer _explorer;

public:
    //! Creates an empty explorer, becomes useful after Init.
    ocgd_TopExp_Explorer();

    //! Destructor.
    virtual ~ocgd_TopExp_Explorer();

    //! Creates an Explorer on the Shape S.
    //! ToFind is the type of shapes to search.
    //! TopAbs_VERTEX, TopAbs_EDGE, ...
    //! ToAvoid is the type of shape to skip in the exploration.
    //! If ToAvoid is equal or less complex than ToFind or if ToAvoid is SHAPE
    //! it has no effect on the exploration.
    void init(const Ref<ocgd_TopoDS_Shape>& shape, int to_find, int to_avoid = TopAbs_SHAPE);

    //! Returns True if there are more shapes in the exploration.
    bool more() const;

    //! Moves to the next Shape in the exploration.
    //! Exceptions: Standard_NoMoreObject if there are no more shapes to explore.
    void next();

    //! Returns the current shape in the exploration.
    //! Exceptions: Standard_NoSuchObject if this explorer has no more shapes to explore.
    Ref<ocgd_TopoDS_Shape> current() const;

    //! Returns the current shape in the exploration (alias for current).
    //! Exceptions: Standard_NoSuchObject if this explorer has no more shapes to explore.
    Ref<ocgd_TopoDS_Shape> value() const;

    //! Reinitialize the exploration with the original arguments.
    void re_init();

    //! Return explored shape.
    Ref<ocgd_TopoDS_Shape> explored_shape() const;

    //! Returns the current depth of the exploration. 0 is the shape to explore itself.
    int depth() const;

    //! Clears the content of the explorer. It will return False on More().
    void clear();

    // Shape type enumeration constants for GDScript (mirrors TopoDS_Shape)
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

    // Internal access to the wrapped OpenCASCADE object
    const TopExp_Explorer& get_occt_explorer() const { return _explorer; }
    TopExp_Explorer& get_occt_explorer() { return _explorer; }
};

VARIANT_ENUM_CAST(ocgd_TopExp_Explorer::ShapeType);

#endif // _ocgd_TopExp_Explorer_HeaderFile