#ifndef OCC_ELIPS_HXX
#define OCC_ELIPS_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Elips.hxx>

class OCCElips : public godot::RefCounted {
    GDCLASS(OCCElips, godot::RefCounted);
private:
    gp_Elips occ_elips;
protected:
    static void _bind_methods();
public:
    OCCElips();
    OCCElips(double px, double py, double pz, double nx, double ny, double nz, double major, double minor);
    void set_origin_normal_radii(double px, double py, double pz, double nx, double ny, double nz, double major, double minor);
    godot::TypedArray<double> get_origin_normal_radii() const;
};

#endif // OCC_ELIPS_HXX
