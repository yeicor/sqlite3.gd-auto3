#include "OCCDir.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

OCCDir::OCCDir() : occ_dir(1,0,0) {} // Default direction (1,0,0)
OCCDir::OCCDir(double x, double y, double z) : occ_dir(x, y, z) {}
OCCDir::OCCDir(const godot::Ref<OCCVec>& vec) : occ_dir(vec->get_x(), vec->get_y(), vec->get_z()) {}
OCCDir::OCCDir(const godot::Ref<OCCPnt>& pnt) : occ_dir(pnt->get_x(), pnt->get_y(), pnt->get_z()) {}

void OCCDir::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_xyz", "x", "y", "z"), &OCCDir::set_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_xyz"), &OCCDir::get_xyz);
    godot::ClassDB::bind_method(godot::D_METHOD("get_x"), &OCCDir::get_x);
    godot::ClassDB::bind_method(godot::D_METHOD("set_x", "x"), &OCCDir::set_x);
    godot::ClassDB::bind_method(godot::D_METHOD("get_y"), &OCCDir::get_y);
    godot::ClassDB::bind_method(godot::D_METHOD("set_y", "y"), &OCCDir::set_y);
    godot::ClassDB::bind_method(godot::D_METHOD("get_z"), &OCCDir::get_z);
    godot::ClassDB::bind_method(godot::D_METHOD("set_z", "z"), &OCCDir::set_z);
    godot::ClassDB::bind_method(godot::D_METHOD("magnitude"), &OCCDir::magnitude);
    godot::ClassDB::bind_method(godot::D_METHOD("to_array"), &OCCDir::to_array);
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCDir::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCDir::get_type);
}

void OCCDir::set_xyz(double x, double y, double z) {
    occ_dir.SetCoord(x, y, z);
}
godot::TypedArray<double> OCCDir::get_xyz() const {
    godot::TypedArray<double> arr;
    arr.append(occ_dir.X());
    arr.append(occ_dir.Y());
    arr.append(occ_dir.Z());
    return arr;
}
double OCCDir::get_x() const { return occ_dir.X(); }
void OCCDir::set_x(double x) { occ_dir.SetX(x); }
double OCCDir::get_y() const { return occ_dir.Y(); }
void OCCDir::set_y(double y) { occ_dir.SetY(y); }
double OCCDir::get_z() const { return occ_dir.Z(); }
void OCCDir::set_z(double z) { occ_dir.SetZ(z); }
double OCCDir::magnitude() const { return 1.0; }
godot::TypedArray<double> OCCDir::to_array() const { return get_xyz(); }
bool OCCDir::is_null() const { return std::abs(occ_dir.X()) < 1e-9 && std::abs(occ_dir.Y()) < 1e-9 && std::abs(occ_dir.Z()) < 1e-9; }
godot::String OCCDir::get_type() const { return "DIR"; }
