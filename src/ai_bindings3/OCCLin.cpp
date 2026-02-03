#include "OCCLin.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCLin::OCCLin() : occ_lin(gp_Pnt(0,0,0), gp_Dir(1,0,0)) {}
OCCLin::OCCLin(double px, double py, double pz, double dx, double dy, double dz) : occ_lin(gp_Pnt(px,py,pz), gp_Dir(dx,dy,dz)) {}
void OCCLin::set_origin_direction(double px, double py, double pz, double dx, double dy, double dz) { occ_lin.SetLocation(gp_Pnt(px,py,pz)); occ_lin.SetDirection(gp_Dir(dx,dy,dz)); }
godot::TypedArray<double> OCCLin::get_origin_direction() const {
    godot::TypedArray<double> arr;
    const gp_Pnt &p = occ_lin.Location();
    const gp_Dir &d = occ_lin.Direction();
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(d.X()); arr.push_back(d.Y()); arr.push_back(d.Z());
    return arr;
}
void OCCLin::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_direction", "px", "py", "pz", "dx", "dy", "dz"), &OCCLin::set_origin_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_direction"), &OCCLin::get_origin_direction);
}
