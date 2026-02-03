#include "OCCShell.hxx"
#include <godot_cpp/core/class_db.hpp>

OCCShell::OCCShell() {}
OCCShell::OCCShell(const TopoDS_Shell &shell) : occ_shell(shell) {}

void OCCShell::_bind_methods() {
    godot::ClassDB::bind_method(godot::D_METHOD("is_null"), &OCCShell::is_null);
    godot::ClassDB::bind_method(godot::D_METHOD("get_type"), &OCCShell::get_type);
}

void OCCShell::set_shell(const TopoDS_Shell &shell) {
    occ_shell = shell;
}

const TopoDS_Shell &OCCShell::get_occ_shell() const {
    return occ_shell;
}

bool OCCShell::is_null() const {
    return occ_shell.IsNull();
}

godot::String OCCShell::get_type() const {
    return occ_shell.IsNull() ? "NULL" : "SHELL";
}
