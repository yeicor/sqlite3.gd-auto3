#ifndef OCC_MAKEEDGE_HXX
#define OCC_MAKEEDGE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include "OCCPnt.hxx"
#include "OCCEdge.hxx"
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/typed_array.hpp>

class OCCMakeEdge : public godot::RefCounted {
    GDCLASS(OCCMakeEdge, godot::RefCounted);

private:
    BRepBuilderAPI_MakeEdge occ_makeedge;

protected:
    static void _bind_methods();

public:
    // Constructors
    OCCMakeEdge();
    OCCMakeEdge(const godot::Ref<OCCPnt>& p1, const godot::Ref<OCCPnt>& p2);

    // Methods
    godot::Ref<OCCEdge> get_edge();
    bool is_done() const;
    godot::String get_error() const;
    godot::String get_type() const;
};

#endif // OCC_MAKEEDGE_HXX
