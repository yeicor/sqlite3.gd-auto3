#include "OCCCylinder.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>

OCCCylinder::OCCCylinder() : occ_cylinder(gp_Ax3(gp_Pnt(0,0,0), gp_Dir(0,0,1), gp_Dir(1,0,0)), 1.0) {}
OCCCylinder::OCCCylinder(double px, double py, double pz, double nx, double ny, double nz, double radius) : occ_cylinder(gp_Ax3(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz), gp_Dir(1,0,0)), radius) {}
void OCCCylinder::set_origin_normal_radius(double px, double py, double pz, double nx, double ny, double nz, double radius) {
    occ_cylinder.SetLocation(gp_Pnt(px,py,pz));
    occ_cylinder.SetAxis(gp_Ax1(gp_Pnt(px,py,pz), gp_Dir(nx,ny,nz)));
    occ_cylinder.SetRadius(radius);
}
godot::TypedArray<double> OCCCylinder::get_origin_normal_radius() const {
    const gp_Pnt &p = occ_cylinder.Location();
    const gp_Dir &n = occ_cylinder.Axis().Direction();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(n.X()); arr.push_back(n.Y()); arr.push_back(n.Z());
    arr.push_back(occ_cylinder.Radius());
    return arr;
}
void OCCCylinder::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_origin_normal_radius", "px", "py", "pz", "nx", "ny", "nz", "radius"), &OCCCylinder::set_origin_normal_radius);
    godot::ClassDB::bind_method(godot::D_METHOD("get_origin_normal_radius"), &OCCCylinder::get_origin_normal_radius);
}
