#include "OCCConeShape.hxx"
#include "OCCShape.hxx"
#include <BRepPrimAPI_MakeCone.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <godot_cpp/core/class_db.hpp>

OCCConeShape::OCCConeShape() : occ_cone_shape(BRepPrimAPI_MakeCone(1.0, 0.0, 1.0).Shape()) {}
OCCConeShape::OCCConeShape(double radius1, double radius2, double height) :
    occ_cone_shape(BRepPrimAPI_MakeCone(radius1, radius2, height).Shape()) {}
OCCConeShape::OCCConeShape(double px, double py, double pz, double radius1, double radius2, double height) :
    occ_cone_shape(BRepPrimAPI_MakeCone(gp_Ax2(gp_Pnt(px, py, pz), gp_Dir(0,0,1)), radius1, radius2, height).Shape()) {}

godot::Ref<OCCShape> OCCConeShape::get_shape() const {
    godot::Ref<OCCShape> shape = godot::Ref<OCCShape>(memnew(OCCShape));
    shape->set_shape(occ_cone_shape);
    return shape;
}

void OCCConeShape::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("get_shape"), &OCCConeShape::get_shape);
}
