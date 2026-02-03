/* godot-cpp OpenCASCADE GDExtension binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_Aspect_FillMethod.hxx"

#include <godot_cpp/core/class_db.hpp>

#ifdef WIN32
#include <windows.h>
#endif

using namespace godot;

//! Defines the fill methods to
//! write bitmaps in a window.

void gd_Aspect_FillMethod::_bind_methods() {
	BIND_ENUM_CONSTANT(FM_NONE);
	BIND_ENUM_CONSTANT(FM_CENTERED);
	BIND_ENUM_CONSTANT(FM_TILED);
	BIND_ENUM_CONSTANT(FM_STRETCH);
}