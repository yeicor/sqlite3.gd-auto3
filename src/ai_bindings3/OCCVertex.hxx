#ifndef OCC_VERTEX_HXX
#define OCC_VERTEX_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Vertex.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCVertex : public godot::RefCounted {
    GDCLASS(OCCVertex, godot::RefCounted);

private:
    TopoDS_Vertex occ_vertex;

protected:
    static void _bind_methods();

public:
    OCCVertex();
    OCCVertex(const TopoDS_Vertex &vertex);

    void set_vertex(const TopoDS_Vertex &vertex);
    const TopoDS_Vertex &get_occ_vertex() const;

    bool is_null() const;
    godot::String get_type() const;
};

#endif // OCC_VERTEX_HXX
