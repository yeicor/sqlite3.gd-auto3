#ifndef OCC_BOX_HXX
#define OCC_BOX_HXX

#include <godot_cpp/classes/ref_counted.hpp>
#include "OCCShape.hxx"

class OCCBox : public godot::RefCounted {
    GDCLASS(OCCBox, godot::RefCounted);

private:
    TopoDS_Shape occ_box;

protected:
    static void _bind_methods();

public:
    OCCBox();
    OCCBox(double dx, double dy, double dz);
    OCCBox(double x_min, double y_min, double z_min, double x_max, double y_max, double z_max);

    godot::Ref<OCCShape> get_shape() const;
};

#endif // OCC_BOX_HXX
