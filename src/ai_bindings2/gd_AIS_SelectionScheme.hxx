#ifndef GD_AIS_SELECTIONSCHEME_HXX
#define GD_AIS_SELECTIONSCHEME_HXX

// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published by
// the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "AIS_SelectionScheme.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//! Sets selection schemes for interactive contexts.
class gd_AIS_SelectionScheme : public RefCounted {
	GDCLASS(gd_AIS_SelectionScheme, RefCounted);

public:
	//! Enum for selection schemes.
	enum SelectionScheme {
		UNKNOWN = -1, //!< undefined scheme
		Replace = 0,  //!< clears current selection and select detected objects
		Add,          //!< adds    detected object to current selection
		Remove,       //!< removes detected object from the current selection
		XOR,          //!< performs XOR for detected objects, other selected not touched
		Clear,        //!< clears current selection
		ReplaceExtra, //!< replace with one difference: if result of replace is an
		              //!< empty, and current selection contains detected element, it
		              //!< will be selected
	};

protected:
	static void _bind_methods();

private:
	// No underlying C++ object pointer needed for pure enum wrapper.

public:
	gd_AIS_SelectionScheme();
	~gd_AIS_SelectionScheme();
};

VARIANT_ENUM_CAST(gd_AIS_SelectionScheme::SelectionScheme);

#endif // GD_AIS_SELECTIONSCHEME_HXX