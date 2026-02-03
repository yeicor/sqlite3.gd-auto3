#ifndef OCC_SHELL_HXX
#define OCC_SHELL_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Shell.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCShell : public godot::RefCounted {
    GDCLASS(OCCShell, godot::RefCounted);

private:
    TopoDS_Shell occ_shell;

protected:
    static void _bind_methods();

public:
    OCCShell();
    OCCShell(const TopoDS_Shell &shell);

    void set_shell(const TopoDS_Shell &shell);
    const TopoDS_Shell &get_occ_shell() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_SHELL_HXX
