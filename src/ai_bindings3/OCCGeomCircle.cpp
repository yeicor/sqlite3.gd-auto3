#include "OCCGeomCircle.hxx"
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/string.hpp>

OCCGeomCircle::OCCGeomCircle() {
    occ_circle = new Geom_Circle(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), 1.0);
}
OCCGeomCircle::OCCGeomCircle(double radius) {
    occ_circle = new Geom_Circle(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), radius);
}
OCCGeomCircle::OCCGeomCircle(double radius, double x, double y, double z) {
    occ_circle = new Geom_Circle(gp_Ax2(gp_Pnt(x, y, z), gp_Dir(0,0,1)), radius);
}
OCCGeomCircle::OCCGeomCircle(const godot::TypedArray<double>& axis, double radius) {
    if (axis.size() < 6) {
        occ_circle = new Geom_Circle(gp_Ax2(gp_Pnt(0,0,0), gp_Dir(0,0,1)), radius);
    } else {
        occ_circle = new Geom_Circle(
            gp_Ax2(
                gp_Pnt(axis[0], axis[1], axis[2]),
                gp_Dir(axis[3], axis[4], axis[5])
            ), radius
        );
    }
}

bool OCCGeomCircle::is_null() const {
    return occ_circle.IsNull();
}
godot::String OCCGeomCircle::get_type() const {
    return "GeomCircle";
}
double OCCGeomCircle::get_radius() const {
    if (occ_circle.IsNull()) return 0.0;
    return occ_circle->Radius();
}
void OCCGeomCircle::set_radius(double r) {
    if (!occ_circle.IsNull()) occ_circle->SetRadius(r);
}
godot::TypedArray<double> OCCGeomCircle::get_position() const {
    godot::TypedArray<double> arr;
    if (occ_circle.IsNull()) return arr;
    gp_Ax2 ax = occ_circle->Position();
    gp_Pnt origin = ax.Location();
    gp_Dir dir = ax.Direction();
    arr.append(origin.X()); arr.append(origin.Y()); arr.append(origin.Z());
    arr.append(dir.X()); arr.append(dir.Y()); arr.append(dir.Z());
    return arr;
}
void OCCGeomCircle::set_position(const godot::TypedArray<double>& axis) {
    if (occ_circle.IsNull()) return;
    if (axis.size() < 6) return;
    gp_Pnt origin(axis[0], axis[1], axis[2]);
    gp_Dir dir(axis[3], axis[4], axis[5]);
    occ_circle->SetPosition(gp_Ax2(origin, dir));
}
Handle(Geom_Circle) OCCGeomCircle::get_occ_circle() const {
    return occ_circle;
}

void OCCGeomCircle::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCGeomCircle::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCGeomCircle::get_type);
    godot::ClassDB::bind_method(godot::D_METHOD("get_radius"), &OCCGeomCircle::get_radius);
    godot::ClassDB::bind_method(godot::D_METHOD("set_radius", "radius"), &OCCGeomCircle::set_radius);
    godot::ClassDB::bind_method(godot::D_METHOD("get_position"), &OCCGeomCircle::get_position);
    godot::ClassDB::bind_method(godot::D_METHOD("set_position", "axis"), &OCCGeomCircle::set_position);
}
