#ifndef OCC_COMPOUND_HXX
#define OCC_COMPOUND_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Compound.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCCompound : public godot::RefCounted {
    GDCLASS(OCCCompound, godot::RefCounted);

private:
    TopoDS_Compound occ_compound;

protected:
    static void _bind_methods();

public:
    OCCCompound();
    OCCCompound(const TopoDS_Compound &compound);

    void set_compound(const TopoDS_Compound &compound);
    const TopoDS_Compound &get_occ_compound() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_COMPOUND_HXX
