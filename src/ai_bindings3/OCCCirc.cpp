#include "OCCCirc.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCCirc::OCCCirc() : occ_circ(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1.0) {}
OCCCirc::OCCCirc(double px, double py, double pz, double nx, double ny, double nz, double radius) : occ_circ(gp_Ax2(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)), radius) {}
void OCCCirc::set_origin_normal_radius(double px, double py, double pz, double nx, double ny, double nz, double radius) {
    occ_circ.SetLocation(gp_Pnt(px,py,pz));
    occ_circ.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
    occ_circ.SetRadius(radius);
}
godot::TypedArray<double> OCCCirc::get_origin_normal_radius() const {
    const gp_Pnt &p = occ_circ.Location();
    const gp_Dir &n = occ_circ.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    arr.push_back(occ_circ.Radius());
    return arr;
}
void OCCCirc::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal_radius", "px", "py", "pz", "nx", "ny", "nz", "radius"), &OCCCirc::set_origin_normal_radius);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal_radius"), &OCCCirc::get_origin_normal_radius);
}
