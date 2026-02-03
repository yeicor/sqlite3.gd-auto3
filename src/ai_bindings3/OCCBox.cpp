#include "OCCBox.hxx"
#include "OCCShape.hxx"
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <godot_cpp/core/class_db.hpp>

OCCBox::OCCBox() : occ_box(BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Shape()) {}

OCCBox::OCCBox(double dx, double dy, double dz) : occ_box(BRepPrimAPI_MakeBox(dx, dy, dz).Shape()) {}

OCCBox::OCCBox(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max)
    : occ_box(BRepPrimAPI_MakeBox(gp_Pnt(x_min, y_min, z_min), gp_Pnt(x_max, y_max, z_max)).Shape()) {}

godot::Ref<OCCShape> OCCBox::get_shape() const {
    godot::Ref<OCCShape> shape = godot::Ref<OCCShape>(memnew(OCCShape));
    shape->set_shape(occ_box);
    return shape;
}

void OCCBox::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("get_shape"), &OCCBox::get_shape);
}
