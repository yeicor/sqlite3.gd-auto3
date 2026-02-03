#ifndef OCC_AX2_HXX
#define OCC_AX2_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <godot_cpp/variant/string.hpp>
#include "OCCPnt.hxx"
#include "OCCDir.hxx"

class OCCAx2 : public godot::RefCounted {
    GDCLASS(OCCAx2, godot::RefCounted);
private:
    gp_Ax2 occ_ax2;
protected:
    static void _bind_methods();
public:
    OCCAx2();
    OCCAx2(const godot::Ref<OCCPnt>& pnt, const godot::Ref<OCCDir>& main_dir, const godot::Ref<OCCDir>& x_dir);
    void set_point(const godot::Ref<OCCPnt>& pnt);
    godot::Ref<OCCPnt> get_point() const;
    void set_main_direction(const godot::Ref<OCCDir>& dir);
    godot::Ref<OCCDir> get_main_direction() const;
    void set_x_direction(const godot::Ref<OCCDir>& dir);
    godot::Ref<OCCDir> get_x_direction() const;
    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_AX2_HXX
