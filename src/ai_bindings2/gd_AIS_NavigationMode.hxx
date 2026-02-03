/* godot-cpp OpenCASCADE AIS_NavigationMode binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef GD_AIS_NAVIGATIONMODE_HXX
#define GD_AIS_NAVIGATIONMODE_HXX

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "AIS_NavigationMode.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

//! Camera navigation mode.
//! This class only exposes the enum for Godot, as there are no methods in the original header.
class gd_AIS_NavigationMode : public RefCounted {
	GDCLASS(gd_AIS_NavigationMode, RefCounted);

public:
	//! Camera navigation mode.
	enum NavigationMode {
		Orbit = AIS_NavigationMode_Orbit,             //!< orbit rotation
		FirstPersonFlight = AIS_NavigationMode_FirstPersonFlight, //!< flight rotation (first person)
		FirstPersonWalk = AIS_NavigationMode_FirstPersonWalk,   //!< walking mode (first person)
	};

	enum {
		Lower = AIS_NavigationMode_LOWER,
		Upper = AIS_NavigationMode_UPPER
	};

protected:
	static void _bind_methods();
};

VARIANT_ENUM_CAST(gd_AIS_NavigationMode::NavigationMode);

#endif // GD_AIS_NAVIGATIONMODE_HXX