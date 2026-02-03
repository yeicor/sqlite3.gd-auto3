#ifndef OCCCOMMON_HXX
#define OCCCOMMON_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCCommon : public godot::RefCounted {
    GDCLASS(OCCCommon, godot::RefCounted);

public:
    static godot::Ref<OCCShape> common(const godot::Ref<OCCShape> &a, const godot::Ref<OCCShape> &b);

protected:
    static void _bind_methods();
};

#endif // OCCCOMMON_HXX
