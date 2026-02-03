#ifndef OCC_SPHERESHAPE_HXX
#define OCC_SPHERESHAPE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCSphereShape : public godot::RefCounted {
    GDCLASS(OCCSphereShape, godot::RefCounted);

private:
    TopoDS_Shape occ_sphere_shape;

protected:
    static void _bind_methods();

public:
    OCCSphereShape();
    OCCSphereShape(double radius);
    OCCSphereShape(double px, double py, double pz, double radius);

    godot::Ref<OCCShape> get_shape() const;
};

#endif // OCC_SPHERESHAPE_HXX
