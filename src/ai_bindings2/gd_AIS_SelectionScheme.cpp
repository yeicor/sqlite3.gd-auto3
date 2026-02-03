/* Open CASCADE Technology Godot GDExtension binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_AIS_SelectionScheme.hxx"

#include <godot_cpp/core/class_db.hpp>

#ifdef WIN32
#include <windows.h>
#endif

using namespace godot;

//! Sets selection schemes for interactive contexts.

gd_AIS_SelectionScheme::gd_AIS_SelectionScheme() {
	// No underlying C++ object needed for pure enum wrapper.
}

gd_AIS_SelectionScheme::~gd_AIS_SelectionScheme() {
	// No resources to free.
}

void gd_AIS_SelectionScheme::_bind_methods() {
	// Enum constants.
	BIND_ENUM_CONSTANT(UNKNOWN);
	BIND_ENUM_CONSTANT(Replace);
	BIND_ENUM_CONSTANT(Add);
	BIND_ENUM_CONSTANT(Remove);
	BIND_ENUM_CONSTANT(XOR);
	BIND_ENUM_CONSTANT(Clear);
	BIND_ENUM_CONSTANT(ReplaceExtra);
}