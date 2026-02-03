#include "OCCAx2.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCAx2::OCCAx2() : occ_ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0)) {}
OCCAx2::OCCAx2(const godot::Ref<OCCPnt>& pnt, const godot::Ref<OCCDir>& main_dir, const godot::Ref<OCCDir>& x_dir)
    : occ_ax2(gp_Pnt(pnt->get_x(), pnt->get_y(), pnt->get_z()), gp_Dir(main_dir->get_x(), main_dir->get_y(), main_dir->get_z()), gp_Dir(x_dir->get_x(), x_dir->get_y(), x_dir->get_z())) {}

void OCCAx2::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("set_point", "pnt"), &OCCAx2::set_point);
    godot::ClassDB::bind_method(godot::D_METHOD("get_point"), &OCCAx2::get_point);
    godot::ClassDB::bind_method(godot::D_METHOD("set_main_direction", "dir"), &OCCAx2::set_main_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("get_main_direction"), &OCCAx2::get_main_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("set_x_direction", "dir"), &OCCAx2::set_x_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("get_x_direction"), &OCCAx2::get_x_direction);
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCAx2::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCAx2::get_type);
}

void OCCAx2::set_point(const godot::Ref<OCCPnt>& pnt) {
    occ_ax2.SetLocation(gp_Pnt(pnt->get_x(), pnt->get_y(), pnt->get_z()));
}
godot::Ref<OCCPnt> OCCAx2::get_point() const {
    gp_Pnt p = occ_ax2.Location();
    auto result = godot::Ref<OCCPnt>(memnew(OCCPnt));
    result->set_xyz(p.X(), p.Y(), p.Z());
    return result;
}
void OCCAx2::set_main_direction(const godot::Ref<OCCDir>& dir) {
    occ_ax2.SetDirection(gp_Dir(dir->get_x(), dir->get_y(), dir->get_z()));
}
godot::Ref<OCCDir> OCCAx2::get_main_direction() const {
    gp_Dir d = occ_ax2.Direction();
    auto result = godot::Ref<OCCDir>(memnew(OCCDir));
    result->set_xyz(d.X(), d.Y(), d.Z());
    return result;
}
void OCCAx2::set_x_direction(const godot::Ref<OCCDir>& dir) {
    occ_ax2.SetXDirection(gp_Dir(dir->get_x(), dir->get_y(), dir->get_z()));
}
godot::Ref<OCCDir> OCCAx2::get_x_direction() const {
    gp_Dir d = occ_ax2.XDirection();
    auto result = godot::Ref<OCCDir>(memnew(OCCDir));
    result->set_xyz(d.X(), d.Y(), d.Z());
    return result;
}
bool OCCAx2::is_null() const {
    return (std::abs(occ_ax2.Location().X()) < 1e-9 && std::abs(occ_ax2.Location().Y()) < 1e-9 && std::abs(occ_ax2.Location().Z()) < 1e-9);
}
godot::String OCCAx2::get_type() const {
    return "AX2";
}
