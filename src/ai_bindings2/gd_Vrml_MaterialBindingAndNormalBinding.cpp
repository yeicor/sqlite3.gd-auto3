/* godot-cpp OpenCASCADE GDExtension binding.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * This file is a binding wrapper implementation for gd_Vrml_MaterialBindingAndNormalBinding.hxx.
 */

#include "gd_Vrml_MaterialBindingAndNormalBinding.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

#ifdef WIN32
// Windows-specific code could go here if needed.
#endif

void gd_Vrml_MaterialBindingAndNormalBinding::_bind_methods() {
	// Enum constants binding for Godot.
	BIND_ENUM_CONSTANT(DEFAULT);
	BIND_ENUM_CONSTANT(OVERALL);
	BIND_ENUM_CONSTANT(PER_PART);
	BIND_ENUM_CONSTANT(PER_PART_INDEXED);
	BIND_ENUM_CONSTANT(PER_FACE);
	BIND_ENUM_CONSTANT(PER_FACE_INDEXED);
	BIND_ENUM_CONSTANT(PER_VERTEX);
	BIND_ENUM_CONSTANT(PER_VERTEX_INDEXED);
}