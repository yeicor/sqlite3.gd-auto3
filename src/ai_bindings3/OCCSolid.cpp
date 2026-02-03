#include "OCCSolid.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCSolid::OCCSolid() {}
OCCSolid::OCCSolid(const TopoDS_Solid &solid) : occ_solid(solid) {}

void OCCSolid::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCSolid::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCSolid::get_type);
}

void OCCSolid::set_solid(const TopoDS_Solid &solid) {
    occ_solid = solid;
}

const TopoDS_Solid &OCCSolid::get_occ_solid() const {
    return occ_solid;
}

bool OCCSolid::is_null() const {
    return occ_solid.IsNull();
}

godot::String OCCSolid::get_type() const {
    return occ_solid.IsNull() ? "NULL" : "SOLID";
}
