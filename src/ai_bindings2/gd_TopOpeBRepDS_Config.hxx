/* godot-cpp OpenCASCADE integration binding.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Wrapper for TopOpeBRepDS_Config.hxx
 *
 * Created on: 1993-06-17
 * Created by: Jean Yves LEBEY
 * Copyright (c) 1993-1999 Matra Datavision
 * Copyright (c) 1999-2014 OPEN CASCADE SAS
 *
 * This file is part of Open CASCADE Technology software library.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation, with special exception defined in the file
 * OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 * distribution for complete text of the license and disclaimer of any warranty.
 *
 * Alternatively, this file may be used under the terms of Open CASCADE
 * commercial license or contractual agreement.
 */

#pragma once

#ifndef GD_TOPOPEBREPDS_CONFIG_HXX
#define GD_TOPOPEBREPDS_CONFIG_HXX

#ifdef WIN32
#include <windows.h>
#endif

#include "TopOpeBRepDS_Config.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

// Dummy wrapper class for TopOpeBRepDS_Config enum
class gd_TopOpeBRepDS_Config : public RefCounted {
	GDCLASS(gd_TopOpeBRepDS_Config, RefCounted);

public:
	enum Config {
		UNSHGEOMETRY = TopOpeBRepDS_UNSHGEOMETRY,
		SAMEORIENTED = TopOpeBRepDS_SAMEORIENTED,
		DIFFORIENTED = TopOpeBRepDS_DIFFORIENTED
	};

protected:
	static void _bind_methods();

};

VARIANT_ENUM_CAST(gd_TopOpeBRepDS_Config::Config);

#endif // GD_TOPOPEBREPDS_CONFIG_HXX