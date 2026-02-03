#ifndef OCC_EDGE_HXX
#define OCC_EDGE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <TopoDS_Edge.hxx>
#include <godot_cpp/variant/string.hpp>

class OCCEdge : public godot::RefCounted {
    GDCLASS(OCCEdge, godot::RefCounted);

private:
    TopoDS_Edge occ_edge;

protected:
    static void _bind_methods();

public:
    OCCEdge(); // Default constructor
    OCCEdge(const TopoDS_Edge &edge); // Construct from OCC edge

    void set_edge(const TopoDS_Edge &edge);
    const TopoDS_Edge &get_occ_edge() const;

    bool is_null() const; // True if undefined/null
    godot::String get_type() const;
};

#endif // OCC_EDGE_HXX
