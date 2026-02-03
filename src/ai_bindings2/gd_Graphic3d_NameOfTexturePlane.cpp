/* godot-cpp OpenCASCADE binding: gd_Graphic3d_NameOfTexturePlane.cpp
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_Graphic3d_NameOfTexturePlane.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

#ifdef WIN32
#include <windows.h>
#endif

void gd_Graphic3d_NameOfTexturePlane::_bind_methods() {
	// Enum registration
	BIND_ENUM_CONSTANT(NOTP_XY);
	BIND_ENUM_CONSTANT(NOTP_YZ);
	BIND_ENUM_CONSTANT(NOTP_ZX);
	BIND_ENUM_CONSTANT(NOTP_UNKNOWN);
}