#include "OCCFace.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCFace::OCCFace() {}
OCCFace::OCCFace(const TopoDS_Face &face) : occ_face(face) {}

void OCCFace::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCFace::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCFace::get_type);
}

void OCCFace::set_face(const TopoDS_Face &face) {
    occ_face = face;
}

const TopoDS_Face &OCCFace::get_occ_face() const {
    return occ_face;
}

bool OCCFace::is_null() const {
    return occ_face.IsNull();
}

godot::String OCCFace::get_type() const {
    return occ_face.IsNull() ? "NULL" : "FACE";
}
