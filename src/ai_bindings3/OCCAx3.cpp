#include "OCCAx3.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>

OCCAx3::OCCAx3() : occ_ax3(gp_Pnt(0,0,0), gp_Dir(1,0,0), gp_Dir(0,0,1)) {}
OCCAx3::OCCAx3(double x, double y, double z) : occ_ax3(gp_Pnt(x, y, z), gp_Dir(1,0,0), gp_Dir(0,0,1)) {}
void OCCAx3::set_location(double x, double y, double z) { occ_ax3.SetLocation(gp_Pnt(x, y, z)); }
godot::TypedArray<double> OCCAx3::get_location() const {
    const gp_Pnt &pt = occ_ax3.Location();
    godot::TypedArray<double> arr;
    arr.push_back(pt.X());
    arr.push_back(pt.Y());
    arr.push_back(pt.Z());
    return arr;
}
void OCCAx3::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_location", "x", "y", "z"), &OCCAx3::set_location);
    godot::ClassDB::bind_method(godot::D_METHOD("get_location"), &OCCAx3::get_location);
}
