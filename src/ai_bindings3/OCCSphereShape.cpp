#include "OCCSphereShape.hxx"
#include "OCCShape.hxx"
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp_Pnt.hxx>
#include <godot_cpp/core/class_db.hpp>

OCCSphereShape::OCCSphereShape() : occ_sphere_shape(BRepPrimAPI_MakeSphere(1.0).Shape()) {}
OCCSphereShape::OCCSphereShape(double radius) : occ_sphere_shape(BRepPrimAPI_MakeSphere(radius).Shape()) {}
OCCSphereShape::OCCSphereShape(double px, double py, double pz, double radius) :
    occ_sphere_shape(BRepPrimAPI_MakeSphere(gp_Pnt(px, py, pz), radius).Shape()) {}

godot::Ref<OCCShape> OCCSphereShape::get_shape() const {
    godot::Ref<OCCShape> shape = godot::Ref<OCCShape>(memnew(OCCShape));
    shape->set_shape(occ_sphere_shape);
    return shape;
}

void OCCSphereShape::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("get_shape"), &OCCSphereShape::get_shape);
}
