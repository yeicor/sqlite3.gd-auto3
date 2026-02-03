#include "OCCPnt2d.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>

OCCPnt2d::OCCPnt2d() : occ_pnt2d(0.0, 0.0) {}
OCCPnt2d::OCCPnt2d(double x, double y) : occ_pnt2d(x, y) {}
void OCCPnt2d::set_xy(double x, double y) { occ_pnt2d.SetCoord(x, y); }
godot::TypedArray<double> OCCPnt2d::get_xy() const {
    godot::TypedArray<double> arr;
    arr.push_back(occ_pnt2d.X());
    arr.push_back(occ_pnt2d.Y());
    return arr;
}
void OCCPnt2d::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_xy", "x", "y"), &OCCPnt2d::set_xy);
    godot::ClassDB::bind_method(godot::D_METHOD("get_xy"), &OCCPnt2d::get_xy);
}
