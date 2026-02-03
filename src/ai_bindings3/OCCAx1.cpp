#include "OCCAx1.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCAx1::OCCAx1() : occ_ax1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)) {}
OCCAx1::OCCAx1(const godot::Ref<OCCPnt>& pnt, const godot::Ref<OCCDir>& dir)
    : occ_ax1(gp_Pnt(pnt->get_x(), pnt->get_y(), pnt->get_z()), gp_Dir(dir->get_x(), dir->get_y(), dir->get_z())) {}

void OCCAx1::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_point", "pnt"), &OCCAx1::set_point);
    godot::ClassDB::bind_method(godot::D_METHOD("get_point"), &OCCAx1::get_point);
    godot::ClassDB::bind_method(godot::D_METHOD("set_direction", "dir"), &OCCAx1::set_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("get_direction"), &OCCAx1::get_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCAx1::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCAx1::get_type);
}

void OCCAx1::set_point(const godot::Ref<OCCPnt>& pnt) {
    occ_ax1.SetLocation(gp_Pnt(pnt->get_x(), pnt->get_y(), pnt->get_z()));
}
godot::Ref<OCCPnt> OCCAx1::get_point() const {
    gp_Pnt p = occ_ax1.Location();
    auto result = godot::Ref<OCCPnt>(memnew(OCCPnt));
    result->set_xyz(p.X(), p.Y(), p.Z());
    return result;
}
void OCCAx1::set_direction(const godot::Ref<OCCDir>& dir) {
    occ_ax1.SetDirection(gp_Dir(dir->get_x(), dir->get_y(), dir->get_z()));
}
godot::Ref<OCCDir> OCCAx1::get_direction() const {
    gp_Dir d = occ_ax1.Direction();
    auto result = godot::Ref<OCCDir>(memnew(OCCDir));
    result->set_xyz(d.X(), d.Y(), d.Z());
    return result;
}
bool OCCAx1::is_null() const {
    return (std::abs(occ_ax1.Location().X()) < 1e-9 && std::abs(occ_ax1.Location().Y()) < 1e-9 && std::abs(occ_ax1.Location().Z()) < 1e-9);
}
godot::String OCCAx1::get_type() const {
    return "AX1";
}
