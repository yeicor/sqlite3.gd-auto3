#ifndef OCC_PLN_HXX
#define OCC_PLN_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Pln.hxx>

class OCCPln : public godot::RefCounted {
    GDCLASS(OCCPln, godot::RefCounted);
private:
    gp_Pln occ_pln;
protected:
    static void _bind_methods();
public:
    OCCPln();
    OCCPln(double px, double py, double pz, double nx, double ny, double nz);
    void set_origin_normal(double px, double py, double pz, double nx, double ny, double nz);
    godot::TypedArray<double> get_origin_normal() const;
};

#endif // OCC_PLN_HXX
