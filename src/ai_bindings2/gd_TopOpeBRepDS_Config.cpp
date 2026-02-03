/* godot-cpp OpenCASCADE integration binding.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Implementation for gd_TopOpeBRepDS_Config.hxx
 */

#include "gd_TopOpeBRepDS_Config.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

#ifdef WIN32
#include <windows.h>
#endif

void gd_TopOpeBRepDS_Config::_bind_methods() {
	// Enum registration
	BIND_ENUM_CONSTANT(UNSHGEOMETRY);
	BIND_ENUM_CONSTANT(SAMEORIENTED);
	BIND_ENUM_CONSTANT(DIFFORIENTED);
}