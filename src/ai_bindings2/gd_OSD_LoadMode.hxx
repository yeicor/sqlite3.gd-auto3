/* godot-cpp OpenCASCADE OSD_LoadMode binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#pragma once

#ifndef GD_OSD_LOADMODE_HXX
#define GD_OSD_LOADMODE_HXX

#include "OSD_LoadMode.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//! This enumeration is used to load shareable libraries.
class gd_OSD_LoadMode : public RefCounted {
	GDCLASS(gd_OSD_LoadMode, RefCounted);

public:
	enum LoadMode {
		RTLD_LAZY = OSD_RTLD_LAZY,
		RTLD_NOW = OSD_RTLD_NOW
	};

protected:
	static void _bind_methods();

private:
	// No underlying C++ object pointer needed, as this is just an enum wrapper.
};

VARIANT_ENUM_CAST(gd_OSD_LoadMode::LoadMode);

#endif // GD_OSD_LOADMODE_HXX