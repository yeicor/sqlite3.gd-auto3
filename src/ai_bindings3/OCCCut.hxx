#ifndef OCCCUT_HXX
#define OCCCUT_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCCut : public godot::RefCounted {
    GDCLASS(OCCCut, godot::RefCounted);

public:
    static godot::Ref<OCCShape> cut(const godot::Ref<OCCShape> &a, const godot::Ref<OCCShape> &b);

protected:
    static void _bind_methods();
};

#endif // OCCCUT_HXX
