#ifndef OCC_FACE_HXX
#define OCC_FACE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Face.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCFace : public godot::RefCounted {
    GDCLASS(OCCFace, godot::RefCounted);

private:
    TopoDS_Face occ_face;

protected:
    static void _bind_methods();

public:
    OCCFace();
    OCCFace(const TopoDS_Face &face);
    void set_face(const TopoDS_Face &face);
    const TopoDS_Face &get_occ_face() const;
    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_FACE_HXX
