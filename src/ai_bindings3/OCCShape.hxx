#ifndef OCC_SHAPE_HXX
#define OCC_SHAPE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <TopoDS_Shape.hxx>

class OCCShape : public godot::RefCounted {
    GDCLASS(OCCShape, godot::RefCounted);

private:
    TopoDS_Shape occ_shape;

protected:
    static void _bind_methods();

public:
    OCCShape();
    OCCShape(const TopoDS_Shape &shape);
    void set_shape(const TopoDS_Shape &shape);
    const TopoDS_Shape &get_occ_shape() const;

    bool is_null() const;
    godot::String get_type() const;

    bool is_solid() const;
    bool is_compound() const;
    bool is_face() const;
    bool is_edge() const;
    bool is_vertex() const;
};

#endif // OCC_SHAPE_HXX
