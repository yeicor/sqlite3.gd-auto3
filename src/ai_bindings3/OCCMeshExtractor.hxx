#ifndef OCCMESHEXTRACTOR_HXX
#define OCCMESHEXTRACTOR_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"
#include <godot_cpp/variant/dictionary.hpp>

class OCCMeshExtractor : public godot::RefCounted {
    GDCLASS(OCCMeshExtractor, godot::RefCounted);

public:
    // Extract mesh: returns array of dictionaries {vertices, indices}
    static godot::TypedArray<godot::Dictionary> extract_mesh(const godot::Ref<OCCShape> &shape, double deflection = 0.01);

protected:
    static void _bind_methods();
};

#endif // OCCMESHEXTRACTOR_HXX
