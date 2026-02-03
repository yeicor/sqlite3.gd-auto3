#include "OCCCompSolid.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCCompSolid::OCCCompSolid() {}
OCCCompSolid::OCCCompSolid(const TopoDS_CompSolid &compsolid) : occ_compsolid(compsolid) {}

void OCCCompSolid::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCCompSolid::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCCompSolid::get_type);
}

void OCCCompSolid::set_compsolid(const TopoDS_CompSolid &compsolid) {
    occ_compsolid = compsolid;
}

const TopoDS_CompSolid &OCCCompSolid::get_occ_compsolid() const {
    return occ_compsolid;
}

bool OCCCompSolid::is_null() const {
    return occ_compsolid.IsNull();
}

godot::String OCCCompSolid::get_type() const {
    return occ_compsolid.IsNull() ? "NULL" : "COMP_SOLID";
}
