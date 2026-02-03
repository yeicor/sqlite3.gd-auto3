#ifndef OCC_SOLID_HXX
#define OCC_SOLID_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Solid.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCSolid : public godot::RefCounted {
    GDCLASS(OCCSolid, godot::RefCounted);

private:
    TopoDS_Solid occ_solid;

protected:
    static void _bind_methods();

public:
    OCCSolid();
    OCCSolid(const TopoDS_Solid &solid);

    void set_solid(const TopoDS_Solid &solid);
    const TopoDS_Solid &get_occ_solid() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_SOLID_HXX
