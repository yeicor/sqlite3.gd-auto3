#include "OCCPnt.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

OCCPnt::OCCPnt() : occ_pnt(0,0,0) {}
OCCPnt::OCCPnt(double x, double y, double z) : occ_pnt(x, y, z) {}

void OCCPnt::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_xyz", "x", "y", "z"), &OCCPnt::set_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_xyz"), &OCCPnt::get_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_x"), &OCCPnt::get_x);
    godot::ClassDB::bind_method(godot::D_METHOD("set_x", "x"), &OCCPnt::set_x);
    godot::ClassDB::bind_method(godot::D_METHOD("get_y"), &OCCPnt::get_y);
    godot::ClassDB::bind_method(godot::D_METHOD("set_y", "y"), &OCCPnt::set_y);
    godot::ClassDB::bind_method(godot::D_METHOD("get_z"), &OCCPnt::get_z);
    godot::ClassDB::bind_method(godot::D_METHOD("set_z", "z"), &OCCPnt::set_z);
    godot::ClassDB::bind_method(godot::D_METHOD("distance_to", "other"), &OCCPnt::distance_to);
    godot::ClassDB::bind_method(godot::D_METHOD("is_equal", "other", "tol"), &OCCPnt::is_equal);
    godot::ClassDB::bind_method(godot::D_METHOD("to_array"), &OCCPnt::to_array);
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCPnt::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCPnt::get_type);
}

void OCCPnt::set_xyz(double x, double y, double z) {
    occ_pnt.SetCoord(x, y, z);
}
godot::TypedArray<double> OCCPnt::get_xyz() const {
    godot::TypedArray<double> arr;
    arr.append(occ_pnt.X());
    arr.append(occ_pnt.Y());
    arr.append(occ_pnt.Z());
    return arr;
}
double OCCPnt::get_x() const { return occ_pnt.X(); }
void OCCPnt::set_x(double x) { occ_pnt.SetX(x); }
double OCCPnt::get_y() const { return occ_pnt.Y(); }
void OCCPnt::set_y(double y) { occ_pnt.SetY(y); }
double OCCPnt::get_z() const { return occ_pnt.Z(); }
void OCCPnt::set_z(double z) { occ_pnt.SetZ(z); }
double OCCPnt::distance_to(const godot::Ref<OCCPnt>& other) const {
    return occ_pnt.Distance(other->occ_pnt);
}
bool OCCPnt::is_equal(const godot::Ref<OCCPnt>& other, double tol) const {
    return occ_pnt.IsEqual(other->occ_pnt, tol);
}
godot::TypedArray<double> OCCPnt::to_array() const { return get_xyz(); }
bool OCCPnt::is_null() const { return false; }
godot::String OCCPnt::get_type() const { return "PNT"; }
