#ifndef OCC_AX1_HXX
#define OCC_AX1_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <godot_cpp/variant/string.hpp>
#include "OCCPnt.hxx"
#include "OCCDir.hxx"

class OCCAx1 : public godot::RefCounted {
    GDCLASS(OCCAx1, godot::RefCounted);
private:
    gp_Ax1 occ_ax1;
protected:
    static void _bind_methods();
public:
    OCCAx1();
    OCCAx1(const godot::Ref<OCCPnt>& pnt, const godot::Ref<OCCDir>& dir);
    void set_point(const godot::Ref<OCCPnt>& pnt);
    godot::Ref<OCCPnt> get_point() const;
    void set_direction(const godot::Ref<OCCDir>& dir);
    godot::Ref<OCCDir> get_direction() const;
    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_AX1_HXX
