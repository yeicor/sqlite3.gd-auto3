#ifndef OCCFUSE_HXX
#define OCCFUSE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCFuse : public godot::RefCounted {
    GDCLASS(OCCFuse, godot::RefCounted);

public:
    static godot::Ref<OCCShape> fuse(const godot::Ref<OCCShape> &a, const godot::Ref<OCCShape> &b);

protected:
    static void _bind_methods();
};

#endif // OCCFUSE_HXX
