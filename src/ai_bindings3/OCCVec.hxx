#ifndef OCC_VEC_HXX
#define OCC_VEC_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Vec.hxx>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/ref.hpp>

class OCCVec : public godot::RefCounted {
    GDCLASS(OCCVec, godot::RefCounted);
private:
    gp_Vec occ_vec;
protected:
    static void _bind_methods();
public:
    OCCVec();
    OCCVec(double x, double y, double z);
    void set_xyz(double x, double y, double z);
    godot::TypedArray<double> get_xyz() const;
    double get_x() const;
    void set_x(double x);
    double get_y() const;
    void set_y(double y);
    double get_z() const;
    void set_z(double z);
    double magnitude() const;
    double dot(const godot::Ref<OCCVec> &other) const;
    godot::Ref<OCCVec> cross(const godot::Ref<OCCVec> &other) const;
    godot::Ref<OCCVec> add(const godot::Ref<OCCVec> &other) const;
    godot::Ref<OCCVec> sub(const godot::Ref<OCCVec> &other) const;
    void normalize();
    godot::TypedArray<double> to_array() const;
    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_VEC_HXX
