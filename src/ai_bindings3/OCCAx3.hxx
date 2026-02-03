#ifndef OCC_AX3_HXX
#define OCC_AX3_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Ax3.hxx>

class OCCAx3 : public godot::RefCounted {
    GDCLASS(OCCAx3, godot::RefCounted);
private:
    gp_Ax3 occ_ax3;
protected:
    static void _bind_methods();
public:
    OCCAx3();
    OCCAx3(double x, double y, double z);
    void set_location(double x, double y, double z);
    godot::TypedArray<double> get_location() const;
};

#endif // OCC_AX3_HXX
