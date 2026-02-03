#include "OCCEdge.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCEdge::OCCEdge() {} // Default constructed edge
OCCEdge::OCCEdge(const TopoDS_Edge &edge) : occ_edge(edge) {}

void OCCEdge::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCEdge::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCEdge::get_type);
}

void OCCEdge::set_edge(const TopoDS_Edge &edge) {
    occ_edge = edge;
}

const TopoDS_Edge &OCCEdge::get_occ_edge() const {
    return occ_edge;
}

bool OCCEdge::is_null() const {
    return occ_edge.IsNull();
}

godot::String OCCEdge::get_type() const {
    return occ_edge.IsNull() ? "NULL" : "EDGE";
}
