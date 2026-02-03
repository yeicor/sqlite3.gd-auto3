/* godot-cpp OpenCASCADE V3d_StereoDumpOptions binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_V3d_StereoDumpOptions.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

#ifdef WIN32
#include <windows.h>
#endif

gd_V3d_StereoDumpOptions::gd_V3d_StereoDumpOptions() {
	// No underlying C++ object needed for enum wrapper.
}

gd_V3d_StereoDumpOptions::~gd_V3d_StereoDumpOptions() {
	// No resources to free.
}

void gd_V3d_StereoDumpOptions::_bind_methods() {
	// Bind enum values for Godot.
	BIND_ENUM_CONSTANT(SDO_MONO);
	BIND_ENUM_CONSTANT(SDO_LEFT_EYE);
	BIND_ENUM_CONSTANT(SDO_RIGHT_EYE);
	BIND_ENUM_CONSTANT(SDO_BLENDED);
}