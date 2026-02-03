/* godot-cpp OpenCASCADE GDExtension binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef GD_ASPECT_FILLMETHOD_HXX
#define GD_ASPECT_FILLMETHOD_HXX

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "Aspect_FillMethod.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//! Defines the fill methods to
//! write bitmaps in a window.
class gd_Aspect_FillMethod : public RefCounted {
	GDCLASS(gd_Aspect_FillMethod, RefCounted);

public:
	enum FillMethod {
		FM_NONE = Aspect_FM_NONE,
		FM_CENTERED = Aspect_FM_CENTERED,
		FM_TILED = Aspect_FM_TILED,
		FM_STRETCH = Aspect_FM_STRETCH
	};

protected:
	static void _bind_methods();

private:
	// No underlying C++ object pointer needed, as this is just an enum wrapper.
};

VARIANT_ENUM_CAST(gd_Aspect_FillMethod::FillMethod);

#endif // GD_ASPECT_FILLMETHOD_HXX