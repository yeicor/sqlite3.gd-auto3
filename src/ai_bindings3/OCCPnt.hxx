#ifndef OCC_PNT_HXX
#define OCC_PNT_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Pnt.hxx>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

class OCCPnt : public godot::RefCounted {
    GDCLASS(OCCPnt, godot::RefCounted);

private:
    gp_Pnt occ_pnt;

protected:
    static void _bind_methods();

public:
    OCCPnt(); // Default at origin
    OCCPnt(double x, double y, double z);
    void set_xyz(double x, double y, double z);
    godot::TypedArray<double> get_xyz() const;

    double get_x() const;
    void set_x(double x);
    double get_y() const;
    void set_y(double y);
    double get_z() const;
    void set_z(double z);

    double distance_to(const godot::Ref<OCCPnt> &other) const;
    bool is_equal(const godot::Ref<OCCPnt> &other, double tol = 1e-9) const;

    godot::TypedArray<double> to_array() const;

    bool is_null() const; // Always false for gp_Pnt
    godot::String get_type() const;
};

#endif // OCC_PNT_HXX
