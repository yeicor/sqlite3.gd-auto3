#ifndef OCCCYLINDERSHAPE_HXX
#define OCCCYLINDERSHAPE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCCylinderShape : public godot::RefCounted {
    GDCLASS(OCCCylinderShape, godot::RefCounted);

private:
    TopoDS_Shape occ_cylinder_shape;

protected:
    static void _bind_methods();

public:
    OCCCylinderShape();
    OCCCylinderShape(double radius, double height);
    OCCCylinderShape(double px, double py, double pz, double radius, double height);

    godot::Ref<OCCShape> get_shape() const;
};

#endif // OCCCYLINDERSHAPE_HXX
