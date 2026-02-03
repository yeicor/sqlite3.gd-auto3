#include "OCCPln.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCPln::OCCPln() : occ_pln(gp_Pnt(0,0,0), gp_Dir(0,0,1)) {}
OCCPln::OCCPln(double px, double py, double pz, double nx, double ny, double nz) : occ_pln(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)) {}
void OCCPln::set_origin_normal(double px, double py, double pz, double nx, double ny, double nz) {
    occ_pln.SetLocation(gp_Pnt(px,py,pz));
    occ_pln.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
}
godot::TypedArray<double> OCCPln::get_origin_normal() const {
    const gp_Pnt &p = occ_pln.Location();
    const gp_Dir &n = occ_pln.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    return arr;
}
void OCCPln::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal", "px", "py", "pz", "nx", "ny", "nz"), &OCCPln::set_origin_normal);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal"), &OCCPln::get_origin_normal);
}
