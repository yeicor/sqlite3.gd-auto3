#ifndef OCC_COMPSOLID_HXX
#define OCC_COMPSOLID_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_CompSolid.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCCompSolid : public godot::RefCounted {
    GDCLASS(OCCCompSolid, godot::RefCounted);

private:
    TopoDS_CompSolid occ_compsolid;

protected:
    static void _bind_methods();

public:
    OCCCompSolid();
    OCCCompSolid(const TopoDS_CompSolid &compsolid);

    void set_compsolid(const TopoDS_CompSolid &compsolid);
    const TopoDS_CompSolid &get_occ_compsolid() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_COMPSOLID_HXX
