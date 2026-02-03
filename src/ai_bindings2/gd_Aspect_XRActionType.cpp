/* Godot GDExtension binding for Aspect_XRActionType.
 *
 * This file is generated to provide direct bindings for the original C++ enum.
 * 
 * Copyright (c) 2020 OPEN CASCADE SAS
 */

#include "gd_Aspect_XRActionType.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//! XR action type.
void gd_Aspect_XRActionType::_bind_methods() {
	// Enum registration.
	BIND_ENUM_CONSTANT(InputDigital);
	BIND_ENUM_CONSTANT(InputAnalog);
	BIND_ENUM_CONSTANT(InputPose);
	BIND_ENUM_CONSTANT(InputSkeletal);
	BIND_ENUM_CONSTANT(OutputHaptic);
}