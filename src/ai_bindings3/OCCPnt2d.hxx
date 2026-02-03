#ifndef OCC_PNT2D_HXX
#define OCC_PNT2D_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Pnt2d.hxx>

class OCCPnt2d : public godot::RefCounted {
    GDCLASS(OCCPnt2d, godot::RefCounted);
private:
    gp_Pnt2d occ_pnt2d;
protected:
    static void _bind_methods();
public:
    OCCPnt2d();
    OCCPnt2d(double x, double y);
    void set_xy(double x, double y);
    godot::TypedArray<double> get_xy() const;
};

#endif // OCC_PNT2D_HXX
