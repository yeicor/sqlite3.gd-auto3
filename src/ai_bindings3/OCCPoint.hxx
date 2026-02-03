#ifndef OCC_POINT_HXX
#define OCC_POINT_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Pnt.hxx>

namespace godot {

class OCCPoint : public godot::RefCounted {
    GDCLASS(OCCPoint, godot::RefCounted);

private:
    gp_Pnt occ_pnt;

protected:
    static void _bind_methods();

public:
    OCCPoint(); // Default at origin
    OCCPoint(double x, double y, double z);

    void set_xyz(double x, double y, double z);
    TypedArray<double> get_xyz() const;

    double get_x() const;
    void set_x(double x);
    double get_y() const;
    void set_y(double y);
    double get_z() const;
    void set_z(double z);

    double distance_to(const Ref<OCCPoint> &other) const;
    bool is_equal(const Ref<OCCPoint> &other, double tol = 1e-6) const;

    TypedArray<double> to_array() const;

};

}

#endif // OCC_POINT_HXX
