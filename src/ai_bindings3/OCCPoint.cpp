#include "OCCPoint.hxx"

using namespace godot;

OCCPoint::OCCPoint() : occ_pnt(0.0, 0.0, 0.0) {}
OCCPoint::OCCPoint(double x, double y, double z) : occ_pnt(x, y, z) {}

void OCCPoint::set_xyz(double x, double y, double z) {
    occ_pnt.SetCoord(x, y, z);
}

TypedArray<double> OCCPoint::get_xyz() const {
    TypedArray<double> arr;
    arr.push_back(occ_pnt.X());
    arr.push_back(occ_pnt.Y());
    arr.push_back(occ_pnt.Z());
    return arr;
}

double OCCPoint::get_x() const {
    return occ_pnt.X();
}
void OCCPoint::set_x(double x) {
    occ_pnt.SetX(x);
}
double OCCPoint::get_y() const {
    return occ_pnt.Y();
}
void OCCPoint::set_y(double y) {
    occ_pnt.SetY(y);
}
double OCCPoint::get_z() const {
    return occ_pnt.Z();
}
void OCCPoint::set_z(double z) {
    occ_pnt.SetZ(z);
}
double OCCPoint::distance_to(const Ref<OCCPoint> &other) const {
    if (other.is_null()) return -1;
    return occ_pnt.Distance(other->occ_pnt);
}
bool OCCPoint::is_equal(const Ref<OCCPoint> &other, double tol) const {
    if (other.is_null()) return false;
    return occ_pnt.IsEqual(other->occ_pnt, tol);
}

TypedArray<double> OCCPoint::to_array() const {
    return get_xyz();
}

void OCCPoint::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_xyz", "x", "y", "z"), &OCCPoint::set_xyz);
    ClassDB::bind_method(D_METHOD("get_xyz"), &OCCPoint::get_xyz);

    ClassDB::bind_method(D_METHOD("get_x"), &OCCPoint::get_x);
    ClassDB::bind_method(D_METHOD("set_x", "x"), &OCCPoint::set_x);
    ClassDB::bind_method(D_METHOD("get_y"), &OCCPoint::get_y);
    ClassDB::bind_method(D_METHOD("set_y", "y"), &OCCPoint::set_y);
    ClassDB::bind_method(D_METHOD("get_z"), &OCCPoint::get_z);
    ClassDB::bind_method(D_METHOD("set_z", "z"), &OCCPoint::set_z);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "x"), "set_x", "get_x");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "y"), "set_y", "get_y");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "z"), "set_z", "get_z");

    ClassDB::bind_method(D_METHOD("distance_to", "other"), &OCCPoint::distance_to);
    ClassDB::bind_method(D_METHOD("is_equal", "other", "tol"), &OCCPoint::is_equal, DEFVAL(1e-6));

    ClassDB::bind_method(D_METHOD("to_array"), &OCCPoint::to_array);

}
