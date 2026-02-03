#include "OCCParab.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCParab::OCCParab() : occ_parab(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1.0) {}
OCCParab::OCCParab(double px, double py, double pz, double nx, double ny, double nz, double focal) : occ_parab(gp_Ax2(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)), focal) {}
void OCCParab::set_origin_normal_focal(double px, double py, double pz, double nx, double ny, double nz, double focal) {
    occ_parab.SetLocation(gp_Pnt(px,py,pz));
    occ_parab.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
    occ_parab.SetFocal(focal);
}
godot::TypedArray<double> OCCParab::get_origin_normal_focal() const {
    const gp_Pnt &p = occ_parab.Location();
    const gp_Dir &n = occ_parab.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    arr.push_back(occ_parab.Focal());
    return arr;
}
void OCCParab::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal_focal", "px", "py", "pz", "nx", "ny", "nz", "focal"), &OCCParab::set_origin_normal_focal);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal_focal"), &OCCParab::get_origin_normal_focal);
}
