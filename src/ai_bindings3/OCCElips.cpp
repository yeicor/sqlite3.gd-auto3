#include "OCCElips.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCElips::OCCElips() : occ_elips(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1.0, 0.5) {}
OCCElips::OCCElips(double px, double py, double pz, double nx, double ny, double nz, double major, double minor) : occ_elips(gp_Ax2(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)), major, minor) {}
void OCCElips::set_origin_normal_radii(double px, double py, double pz, double nx, double ny, double nz, double major, double minor) {
    occ_elips.SetLocation(gp_Pnt(px,py,pz));
    occ_elips.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
    occ_elips.SetMajorRadius(major);
    occ_elips.SetMinorRadius(minor);
}
godot::TypedArray<double> OCCElips::get_origin_normal_radii() const {
    const gp_Pnt &p = occ_elips.Location();
    const gp_Dir &n = occ_elips.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    arr.push_back(occ_elips.MajorRadius());
    arr.push_back(occ_elips.MinorRadius());
    return arr;
}
void OCCElips::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal_radii", "px", "py", "pz", "nx", "ny", "nz", "major", "minor"), &OCCElips::set_origin_normal_radii);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal_radii"), &OCCElips::get_origin_normal_radii);
}
