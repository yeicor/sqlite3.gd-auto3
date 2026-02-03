#include "OCCSphere.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <gp_Pnt.hxx>

OCCSphere::OCCSphere() : occ_sphere(gp_Ax3(gp_Pnt(0,0,0), gp_Dir(1,0,0), gp_Dir(0,0,1)), 1.0) {}
OCCSphere::OCCSphere(double px, double py, double pz, double radius) : occ_sphere(gp_Ax3(gp_Pnt(px,py,pz), gp_Dir(1,0,0), gp_Dir(0,0,1)), radius) {}
void OCCSphere::set_center_radius(double px, double py, double pz, double radius) {
    occ_sphere.SetLocation(gp_Pnt(px,py,pz));
    occ_sphere.SetRadius(radius);
}
godot::TypedArray<double> OCCSphere::get_center_radius() const {
    const gp_Pnt &p = occ_sphere.Location();
    godot::TypedArray<double> arr;
    arr.push_back(p.X()); arr.push_back(p.Y()); arr.push_back(p.Z());
    arr.push_back(occ_sphere.Radius());
    return arr;
}
void OCCSphere::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_center_radius", "px", "py", "pz", "radius"), &OCCSphere::set_center_radius);
    godot::ClassDB::bind_method(godot::D_METHOD("get_center_radius"), &OCCSphere::get_center_radius);
}
