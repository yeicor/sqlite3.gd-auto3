#include "OCCVertex.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCVertex::OCCVertex() {}
OCCVertex::OCCVertex(const TopoDS_Vertex &vertex) : occ_vertex(vertex) {}

void OCCVertex::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCVertex::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCVertex::get_type);
}

void OCCVertex::set_vertex(const TopoDS_Vertex &vertex) {
    occ_vertex = vertex;
}

const TopoDS_Vertex &OCCVertex::get_occ_vertex() const {
    return occ_vertex;
}

bool OCCVertex::is_null() const {
    return occ_vertex.IsNull();
}

godot::String OCCVertex::get_type() const {
    return occ_vertex.IsNull() ? "NULL" : "VERTEX";
}
