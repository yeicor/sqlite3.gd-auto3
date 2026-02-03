#include "OCCCompound.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCCompound::OCCCompound() {}
OCCCompound::OCCCompound(const TopoDS_Compound &compound) : occ_compound(compound) {}

void OCCCompound::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCCompound::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCCompound::get_type);
}

void OCCCompound::set_compound(const TopoDS_Compound &compound) {
    occ_compound = compound;
}

const TopoDS_Compound &OCCCompound::get_occ_compound() const {
    return occ_compound;
}

bool OCCCompound::is_null() const {
    return occ_compound.IsNull();
}

godot::String OCCCompound::get_type() const {
    return occ_compound.IsNull() ? "NULL" : "COMPOUND";
}
