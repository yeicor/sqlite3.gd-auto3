#include "OCCCylinderShape.hxx"
#include "OCCShape.hxx"
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <godot_cpp/core/class_db.hpp>

OCCCylinderShape::OCCCylinderShape() : occ_cylinder_shape(BRepPrimAPI_MakeCylinder(1.0, 1.0).Shape()) {}
OCCCylinderShape::OCCCylinderShape(double radius, double height) : occ_cylinder_shape(BRepPrimAPI_MakeCylinder(radius, height).Shape()) {}
OCCCylinderShape::OCCCylinderShape(double px, double py, double pz, double radius, double height) :
    occ_cylinder_shape(BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(px, py, pz), gp_Dir(0,0,1)), radius, height).Shape()) {}

godot::Ref<OCCShape> OCCCylinderShape::get_shape() const {
    godot::Ref<OCCShape> shape = godot::Ref<OCCShape>(memnew(OCCShape));
    shape->set_shape(occ_cylinder_shape);
    return shape;
}

void OCCCylinderShape::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("get_shape"), &OCCCylinderShape::get_shape);
}
