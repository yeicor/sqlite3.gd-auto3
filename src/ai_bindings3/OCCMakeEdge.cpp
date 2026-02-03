#include "OCCMakeEdge.hxx"
#include <godot_cpp/core/class_db.hpp>
#include <BRepBuilderAPI_EdgeError.hxx>

OCCMakeEdge::OCCMakeEdge() : occ_makeedge() {}
OCCMakeEdge::OCCMakeEdge(const godot::Ref<OCCPnt>& p1, const godot::Ref<OCCPnt>& p2)
    : occ_makeedge(gp_Pnt(p1->get_x(), p1->get_y(), p1->get_z()), gp_Pnt(p2->get_x(), p2->get_y(), p2->get_z())) {}

void OCCMakeEdge::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("get_edge"), &OCCMakeEdge::get_edge);
    godot::ClassDB::bind_method(godot::D_METHOD("is_done"), &OCCMakeEdge::is_done);
    godot::ClassDB::bind_method(godot::D_METHOD("get_error"), &OCCMakeEdge::get_error);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCMakeEdge::get_type);
}

godot::Ref<OCCEdge> OCCMakeEdge::get_edge() {
    auto edge = occ_makeedge.Edge();
    auto result = godot::Ref<OCCEdge>(memnew(OCCEdge));
    result->set_edge(edge);
    return result;
}

bool OCCMakeEdge::is_done() const {
    return occ_makeedge.IsDone();
}

godot::String OCCMakeEdge::get_error() const {
    BRepBuilderAPI_EdgeError error = occ_makeedge.Error();
    switch(error) {
        case BRepBuilderAPI_EdgeDone: return "EdgeDone";
        case BRepBuilderAPI_PointProjectionFailed: return "PointProjectionFailed";
        case BRepBuilderAPI_ParameterOutOfRange: return "ParameterOutOfRange";
        case BRepBuilderAPI_DifferentPointsOnClosedCurve: return "DifferentPointsOnClosedCurve";
        case BRepBuilderAPI_PointWithInfiniteParameter: return "PointWithInfiniteParameter";
        case BRepBuilderAPI_DifferentsPointAndParameter: return "DifferentsPointAndParameter";
        case BRepBuilderAPI_LineThroughIdenticPoints: return "LineThroughIdenticPoints";
        default: return "Unknown";
    }
}

godot::String OCCMakeEdge::get_type() const {
    return "MAKEEDGE";
}
