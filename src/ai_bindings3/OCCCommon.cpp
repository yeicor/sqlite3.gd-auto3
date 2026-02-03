#include "OCCCommon.hxx"
#include "OCCShape.hxx"
#include <BRepAlgoAPI_Common.hxx>
#include <godot_cpp/core/class_db.hpp>

godot::Ref<OCCShape> OCCCommon::common(const godot::Ref<OCCShape> &a, const godot::Ref<OCCShape> &b) {
    if (a.is_null() || b.is_null() || a->is_null() || b->is_null()) return godot::Ref<OCCShape>();
    TopoDS_Shape result = BRepAlgoAPI_Common(a->get_occ_shape(), b->get_occ_shape()).Shape();
    godot::Ref<OCCShape> ret = godot::Ref<OCCShape>(memnew(OCCShape));
    ret->set_shape(result);
    return ret;
}

void OCCCommon::_bind_methods() {
    godot::ClassDB::bind_static_method("OCCCommon", godot::D_METHOD("common", "a", "b"), &OCCCommon::common);
}
