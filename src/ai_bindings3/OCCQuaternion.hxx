#ifndef OCC_QUATERNION_HXX
#define OCC_QUATERNION_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Quaternion.hxx>

class OCCQuaternion : public godot::RefCounted {
    GDCLASS(OCCQuaternion, godot::RefCounted);
private:
    gp_Quaternion occ_quat;
protected:
    static void _bind_methods();
public:
    OCCQuaternion();
    OCCQuaternion(double x, double y, double z, double w);
    void set_xyzw(double x, double y, double z, double w);
    godot::TypedArray<double> get_xyzw() const;
};

#endif // OCC_QUATERNION_HXX
