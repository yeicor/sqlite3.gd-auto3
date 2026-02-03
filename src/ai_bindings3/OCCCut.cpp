#include "OCCCut.hxx"
#include "OCCShape.hxx"
#include <BRepAlgoAPI_Cut.hxx>
#include <godot_cpp/core/class_db.hpp>

godot::Ref<OCCShape> OCCCut::cut(const godot::Ref<OCCShape> &a, const godot::Ref<OCCShape> &b) {
    if (a.is_null() || b.is_null() || a->is_null() || b->is_null()) return godot::Ref<OCCShape>();
    TopoDS_Shape result = BRepAlgoAPI_Cut(a->get_occ_shape(), b->get_occ_shape()).Shape();
    godot::Ref<OCCShape> ret = godot::Ref<OCCShape>(memnew(OCCShape));
    ret->set_shape(result);
    return ret;
}

void OCCCut::_bind_methods() {
    godot::ClassDB::bind_static_method("OCCCut", godot::D_METHOD("cut", "a", "b"), &OCCCut::cut);
}
