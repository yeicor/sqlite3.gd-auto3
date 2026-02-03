#ifndef OCC_SPHERE_HXX
#define OCC_SPHERE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Sphere.hxx>

class OCCSphere : public godot::RefCounted {
    GDCLASS(OCCSphere, godot::RefCounted);
private:
    gp_Sphere occ_sphere;
protected:
    static void _bind_methods();
public:
    OCCSphere();
    OCCSphere(double px, double py, double pz, double radius);
    void set_center_radius(double px, double py, double pz, double radius);
    godot::TypedArray<double> get_center_radius() const;
};

#endif // OCC_SPHERE_HXX
