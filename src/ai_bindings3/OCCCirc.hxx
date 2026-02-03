#ifndef OCC_CIRC_HXX
#define OCC_CIRC_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Circ.hxx>

class OCCCirc : public godot::RefCounted {
    GDCLASS(OCCCirc, godot::RefCounted);
private:
    gp_Circ occ_circ;
protected:
    static void _bind_methods();
public:
    OCCCirc();
    OCCCirc(double px, double py, double pz, double nx, double ny, double nz, double radius);
    void set_origin_normal_radius(double px, double py, double pz, double nx, double ny, double nz, double radius);
    godot::TypedArray<double> get_origin_normal_radius() const;
};

#endif // OCC_CIRC_HXX
