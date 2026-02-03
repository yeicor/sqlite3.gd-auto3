#ifndef GD_ASPECT_XRACTIONTYPE_HXX
#define GD_ASPECT_XRACTIONTYPE_HXX

/* Godot GDExtension binding for Aspect_XRActionType.
 *
 * This file is generated to provide direct bindings for the original C++ enum.
 * 
 * Copyright (c) 2020 OPEN CASCADE SAS
 * 
 * This file is part of Open CASCADE Technology software library.
 * 
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published by
 * the Free Software Foundation, with special exception defined in the file
 * OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 * distribution for complete text of the license and disclaimer of any warranty.
 * 
 * Alternatively, this file may be used under the terms of Open CASCADE
 * commercial license or contractual agreement.
 */

//! XR action type.

#include "Aspect_XRActionType.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class gd_Aspect_XRActionType : public RefCounted {
	GDCLASS(gd_Aspect_XRActionType, RefCounted);

public:
	//! XR action type.
	enum XRActionType {
		InputDigital = Aspect_XRActionType_InputDigital,  //!< boolean input (like button)
		InputAnalog = Aspect_XRActionType_InputAnalog,    //!< analog input (1/2/3 axes)
		InputPose = Aspect_XRActionType_InputPose,        //!< positional input
		InputSkeletal = Aspect_XRActionType_InputSkeletal,//!< skeletal input
		OutputHaptic = Aspect_XRActionType_OutputHaptic   //!< haptic output (vibration)
	};

protected:
	static void _bind_methods();

};

VARIANT_ENUM_CAST(gd_Aspect_XRActionType::XRActionType);

#endif // GD_ASPECT_XRACTIONTYPE_HXX