#include "OCCTorus.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

OCCTorus::OCCTorus() : occ_torus(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 2.0, 0.5) {}
OCCTorus::OCCTorus(double px, double py, double pz, double nx, double ny, double nz, double major, double minor) : occ_torus(gp_Ax2(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)), major, minor) {}
void OCCTorus::set_origin_normal_radii(double px, double py, double pz, double nx, double ny, double nz, double major, double minor) {
    occ_torus.SetLocation(gp_Pnt(px,py,pz));
    occ_torus.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
    occ_torus.SetMajorRadius(major);
    occ_torus.SetMinorRadius(minor);
}
godot::TypedArray<double> OCCTorus::get_origin_normal_radii() const {
    const gp_Pnt &p = occ_torus.Location();
    const gp_Dir &n = occ_torus.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    arr.push_back(occ_torus.MajorRadius());
    arr.push_back(occ_torus.MinorRadius());
    return arr;
}
void OCCTorus::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal_radii", "px", "py", "pz", "nx", "ny", "nz", "major", "minor"), &OCCTorus::set_origin_normal_radii);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal_radii"), &OCCTorus::get_origin_normal_radii);
}
