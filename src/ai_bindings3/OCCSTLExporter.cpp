#include "OCCSTLExporter.hxx"
#include "OCCShape.hxx"
#include <StlAPI_Writer.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <Standard_Stream.hxx>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/core/class_db.hpp>

bool OCCSTLExporter::export_stl(const godot::Ref<OCCShape> &shape, const godot::String &filename, double deflection) {
    if (shape.is_null() || shape->is_null()) return false;

    BRepMesh_IncrementalMesh(shape->get_occ_shape(), deflection);

    StlAPI_Writer writer;
    writer.Write(shape->get_occ_shape(), filename.utf8().get_data());
    return true;
}

void OCCSTLExporter::_bind_methods() {
    godot::ClassDB::bind_static_method("OCCSTLExporter", godot::D_METHOD("export_stl", "shape", "filename", "deflection"), &OCCSTLExporter::export_stl, 0.01);
}
