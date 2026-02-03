#ifndef OCCGEOMCIRCLE_HXX
#define OCCGEOMCIRCLE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <Geom_Circle.hxx>
#include <gp_Ax2.hxx>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/variant/string.hpp>

class OCCGeomCircle : public godot::RefCounted {
    GDCLASS(OCCGeomCircle, godot::RefCounted);

private:
    Handle(Geom_Circle) occ_circle;

protected:
    static void _bind_methods();

public:
    OCCGeomCircle(); // Origin, Z axis, radius 1
    OCCGeomCircle(double radius);
    OCCGeomCircle(double radius, double x, double y, double z);
    OCCGeomCircle(const godot::TypedArray<double>& axis, double radius);

    bool is_null() const;
    godot::String get_type() const;

    double get_radius() const;
    void set_radius(double r);

    godot::TypedArray<double> get_position() const;        // Ax2: origin and axis direction
    void set_position(const godot::TypedArray<double>& axis); // axis: [x, y, z, dx, dy, dz]

    Handle(Geom_Circle) get_occ_circle() const;

};

#endif // OCCGEOMCIRCLE_HXX
