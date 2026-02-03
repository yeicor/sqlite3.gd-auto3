#ifndef OCC_TORUS_HXX
#define OCC_TORUS_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Torus.hxx>

class OCCTorus : public godot::RefCounted {
    GDCLASS(OCCTorus, godot::RefCounted);
private:
    gp_Torus occ_torus;
protected:
    static void _bind_methods();
public:
    OCCTorus();
    OCCTorus(double px, double py, double pz, double nx, double ny, double nz, double major, double minor);
    void set_origin_normal_radii(double px, double py, double pz, double nx, double ny, double nz, double major, double minor);
    godot::TypedArray<double> get_origin_normal_radii() const;
};

#endif // OCC_TORUS_HXX
