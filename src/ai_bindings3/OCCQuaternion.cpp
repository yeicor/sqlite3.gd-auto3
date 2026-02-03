#include "OCCQuaternion.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

OCCQuaternion::OCCQuaternion() : occ_quat(0.0, 0.0, 0.0, 1.0) {}
OCCQuaternion::OCCQuaternion(double x, double y, double z, double w) : occ_quat(x, y, z, w) {}
void OCCQuaternion::set_xyzw(double x, double y, double z, double w) {
    occ_quat.Set(x, y, z, w);
}
godot::TypedArray<double> OCCQuaternion::get_xyzw() const {
    godot::TypedArray<double> arr;
    arr.push_back(occ_quat.X());
    arr.push_back(occ_quat.Y());
    arr.push_back(occ_quat.Z());
    arr.push_back(occ_quat.W());
    return arr;
}
void OCCQuaternion::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_xyzw", "x", "y", "z", "w"), &OCCQuaternion::set_xyzw);
    godot::ClassDB::bind_method(godot::D_METHOD("get_xyzw"), &OCCQuaternion::get_xyzw);
}
