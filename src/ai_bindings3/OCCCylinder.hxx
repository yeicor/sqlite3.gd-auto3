#ifndef OCC_CYLINDER_HXX
#define OCC_CYLINDER_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Cylinder.hxx>

class OCCCylinder : public godot::RefCounted {
    GDCLASS(OCCCylinder, godot::RefCounted);
private:
    gp_Cylinder occ_cylinder;
protected:
    static void _bind_methods();
public:
    OCCCylinder();
    OCCCylinder(double px, double py, double pz, double nx, double ny, double nz, double radius);
    void set_origin_normal_radius(double px, double py, double pz, double nx, double ny, double nz, double radius);
    godot::TypedArray<double> get_origin_normal_radius() const;
};

#endif // OCC_CYLINDER_HXX
