#ifndef OCCCONE_SHAPE_HXX
#define OCCCONE_SHAPE_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCConeShape : public godot::RefCounted {
    GDCLASS(OCCConeShape, godot::RefCounted);

private:
    TopoDS_Shape occ_cone_shape;

protected:
    static void _bind_methods();

public:
    OCCConeShape();
    OCCConeShape(double radius1, double radius2, double height);
    OCCConeShape(double px, double py, double pz, double radius1, double radius2, double height);

    godot::Ref<OCCShape> get_shape() const;
};

#endif // OCCCONE_SHAPE_HXX
