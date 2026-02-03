#ifndef OCC_PARAB_HXX
#define OCC_PARAB_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Parab.hxx>

class OCCParab : public godot::RefCounted {
    GDCLASS(OCCParab, godot::RefCounted);
private:
    gp_Parab occ_parab;
protected:
    static void _bind_methods();
public:
    OCCParab();
    OCCParab(double px, double py, double pz, double nx, double ny, double nz, double focal);
    void set_origin_normal_focal(double px, double py, double pz, double nx, double ny, double nz, double focal);
    godot::TypedArray<double> get_origin_normal_focal() const;
};

#endif // OCC_PARAB_HXX
