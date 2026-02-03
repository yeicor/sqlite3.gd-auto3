#ifndef OCCSTLEXPORTER_HXX
#define OCCSTLEXPORTER_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"
#include <godot_cpp/variant/string.hpp>

class OCCSTLExporter : public godot::RefCounted {
    GDCLASS(OCCSTLExporter, godot::RefCounted);

public:
    static bool export_stl(const godot::Ref<OCCShape> &shape, const godot::String &filename, double deflection = 0.01);

protected:
    static void _bind_methods();
};

#endif // OCCSTLEXPORTER_HXX
