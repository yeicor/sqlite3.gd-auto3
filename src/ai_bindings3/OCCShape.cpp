#include "OCCShape.hxx"
#include <TopAbs_ShapeEnum.hxx>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/string.hpp>

OCCShape::OCCShape() {} // Default constructed shape

void OCCShape::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCShape::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCShape::get_type);

    godot::ClassDB::bind_method(godot::D_METHOD("is_solid"), &OCCShape::is_solid);
    godot::ClassDB::bind_method(godot::D_METHOD("is_compound"), &OCCShape::is_compound);
    godot::ClassDB::bind_method(godot::D_METHOD("is_face"), &OCCShape::is_face);
    godot::ClassDB::bind_method(godot::D_METHOD("is_edge"), &OCCShape::is_edge);
    godot::ClassDB::bind_method(godot::D_METHOD("is_vertex"), &OCCShape::is_vertex);

    godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::BOOL, "is_null"), "", "is_null");
    godot::ClassDB::add_property(get_class_static(), godot::PropertyInfo(godot::Variant::STRING, "type"), "", "get_type");
}

bool OCCShape::is_null() const {
    return occ_shape.IsNull();
}
godot::String OCCShape::get_type() const {
    if (occ_shape.IsNull())
        return "NULL";
    TopAbs_ShapeEnum type = occ_shape.ShapeType();
    switch (type) {
        case TopAbs_COMPOUND: return "COMPOUND";
        case TopAbs_COMPSOLID: return "COMP_SOLID";
        case TopAbs_SOLID: return "SOLID";

        case TopAbs_SHELL: return "SHELL";
        case TopAbs_FACE: return "FACE";
        case TopAbs_WIRE: return "WIRE";
        case TopAbs_EDGE: return "EDGE";
        case TopAbs_VERTEX: return "VERTEX";
        default: return "UNKNOWN";
    }
}

OCCShape::OCCShape(const TopoDS_Shape &shape) { occ_shape = shape; }
void OCCShape::set_shape(const TopoDS_Shape &shape) { occ_shape = shape; }

bool OCCShape::is_solid() const {
    return !occ_shape.IsNull() && occ_shape.ShapeType() == TopAbs_SOLID;
}
bool OCCShape::is_compound() const {
    return !occ_shape.IsNull() && occ_shape.ShapeType() == TopAbs_COMPOUND;
}
bool OCCShape::is_face() const {
    return !occ_shape.IsNull() && occ_shape.ShapeType() == TopAbs_FACE;
}
bool OCCShape::is_edge() const {
    return !occ_shape.IsNull() && occ_shape.ShapeType() == TopAbs_EDGE;
}
bool OCCShape::is_vertex() const {
    return !occ_shape.IsNull() && occ_shape.ShapeType() == TopAbs_VERTEX;
}
const TopoDS_Shape &OCCShape::get_occ_shape() const { return occ_shape; }
