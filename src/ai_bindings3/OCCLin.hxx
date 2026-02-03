#ifndef OCC_LIN_HXX
#define OCC_LIN_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Lin.hxx>

class OCCLin : public godot::RefCounted {
    GDCLASS(OCCLin, godot::RefCounted);
private:
    gp_Lin occ_lin;
protected:
    static void _bind_methods();
public:
    OCCLin();
    OCCLin(double px, double py, double pz, double dx, double dy, double dz);
    void set_origin_direction(double px, double py, double pz, double dx, double dy, double dz);
    godot::TypedArray<double> get_origin_direction() const;
};

#endif // OCC_LIN_HXX
