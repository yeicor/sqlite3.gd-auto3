#include "OCCVec.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <memory>

OCCVec::OCCVec() : occ_vec(0,0,0) {}
OCCVec::OCCVec(double x, double y, double z) : occ_vec(x, y, z) {}

void OCCVec::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_xyz", "x", "y", "z"), &OCCVec::set_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_xyz"), &OCCVec::get_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_x"), &OCCVec::get_x);
    godot::ClassDB::bind_method(godot::D_METHOD("set_x", "x"), &OCCVec::set_x);
    godot::ClassDB::bind_method(godot::D_METHOD("get_y"), &OCCVec::get_y);
    godot::ClassDB::bind_method(godot::D_METHOD("set_y", "y"), &OCCVec::set_y);
    godot::ClassDB::bind_method(godot::D_METHOD("get_z"), &OCCVec::get_z);
    godot::ClassDB::bind_method(godot::D_METHOD("set_z", "z"), &OCCVec::set_z);
    godot::ClassDB::bind_method(godot::D_METHOD("magnitude"), &OCCVec::magnitude);
    godot::ClassDB::bind_method(godot::D_METHOD("dot", "other"), &OCCVec::dot);
    godot::ClassDB::bind_method(godot::D_METHOD("cross", "other"), &OCCVec::cross);
    godot::ClassDB::bind_method(godot::D_METHOD("add", "other"), &OCCVec::add);
    godot::ClassDB::bind_method(godot::D_METHOD("sub", "other"), &OCCVec::sub);
    godot::ClassDB::bind_method(godot::D_METHOD("normalize"), &OCCVec::normalize);
    godot::ClassDB::bind_method(godot::D_METHOD("to_array"), &OCCVec::to_array);
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCVec::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCVec::get_type);
}

void OCCVec::set_xyz(double x, double y, double z) {
    occ_vec.SetX(x);
    occ_vec.SetY(y);
    occ_vec.SetZ(z);
}
godot::TypedArray<double> OCCVec::get_xyz() const {
    godot::TypedArray<double> arr;
    arr.append(occ_vec.X());
    arr.append(occ_vec.Y());
    arr.append(occ_vec.Z());
    return arr;
}
double OCCVec::get_x() const { return occ_vec.X(); }
void OCCVec::set_x(double x) { occ_vec.SetX(x); }
double OCCVec::get_y() const { return occ_vec.Y(); }
void OCCVec::set_y(double y) { occ_vec.SetY(y); }
double OCCVec::get_z() const { return occ_vec.Z(); }
void OCCVec::set_z(double z) { occ_vec.SetZ(z); }
double OCCVec::magnitude() const { return occ_vec.Magnitude(); }
double OCCVec::dot(const godot::Ref<OCCVec>& other) const { return occ_vec.Dot(other->occ_vec); }
godot::Ref<OCCVec> OCCVec::cross(const godot::Ref<OCCVec>& other) const { auto v = godot::Ref<OCCVec>(memnew(OCCVec)); v->occ_vec = occ_vec.Crossed(other->occ_vec); return v; }
godot::Ref<OCCVec> OCCVec::add(const godot::Ref<OCCVec>& other) const { auto v = godot::Ref<OCCVec>(memnew(OCCVec)); v->occ_vec = occ_vec.Added(other->occ_vec); return v; }
godot::Ref<OCCVec> OCCVec::sub(const godot::Ref<OCCVec>& other) const { auto v = godot::Ref<OCCVec>(memnew(OCCVec)); v->occ_vec = occ_vec.Subtracted(other->occ_vec); return v; }
void OCCVec::normalize() { occ_vec.Normalize(); }
godot::TypedArray<double> OCCVec::to_array() const { return get_xyz(); }
bool OCCVec::is_null() const { return occ_vec.Magnitude() == 0.0; }
godot::String OCCVec::get_type() const { return "VEC"; }
