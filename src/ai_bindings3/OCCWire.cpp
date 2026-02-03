#include "OCCWire.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCWire::OCCWire() {}
OCCWire::OCCWire(const TopoDS_Wire &wire) : occ_wire(wire) {}

void OCCWire::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCWire::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCWire::get_type);
}

void OCCWire::set_wire(const TopoDS_Wire &wire) {
    occ_wire = wire;
}

const TopoDS_Wire &OCCWire::get_occ_wire() const {
    return occ_wire;
}

bool OCCWire::is_null() const {
    return occ_wire.IsNull();
}

godot::String OCCWire::get_type() const {
    return occ_wire.IsNull() ? "NULL" : "WIRE";
}
