#ifndef OCC_WIRE_HXX
#define OCC_WIRE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Wire.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCWire : public godot::RefCounted {
    GDCLASS(OCCWire, godot::RefCounted);

private:
    TopoDS_Wire occ_wire;

protected:
    static void _bind_methods();

public:
    OCCWire();
    OCCWire(const TopoDS_Wire &wire);
    void set_wire(const TopoDS_Wire &wire);
    const TopoDS_Wire &get_occ_wire() const;
    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_WIRE_HXX
