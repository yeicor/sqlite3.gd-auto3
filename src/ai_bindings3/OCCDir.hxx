#ifndef OCC_DIR_HXX
#define OCC_DIR_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <godot_cpp/variant/string.hpp>
#include "OCCVec.hxx"
#include "OCCPnt.hxx"

#include <godot_cpp/variant/typed_array.hpp>

class OCCDir : public godot::RefCounted {
    GDCLASS(OCCDir, godot::RefCounted);

private:
    gp_Dir occ_dir;

protected:
    static void _bind_methods();

public:
    OCCDir();
    OCCDir(double x, double y, double z);
    OCCDir(const godot::Ref<OCCVec>& vec);
    OCCDir(const godot::Ref<OCCPnt>& pnt);

    void set_xyz(double x, double y, double z);
    godot::TypedArray<double> get_xyz() const;

    double get_x() const;
    void set_x(double x);
    double get_y() const;
    void set_y(double y);
    double get_z() const;
    void set_z(double z);

    double magnitude() const; // returns 1, here for API symmetry

    godot::TypedArray<double> to_array() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_DIR_HXX
