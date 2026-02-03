/* godot-cpp OpenCASCADE binding: gd_CDF_SubComponentStatus.hxx
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Wrapper for CDF_SubComponentStatus.hxx
 *
 * Copyright (c) 1997-1999 Matra Datavision
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

#ifndef GD_CDF_SUBCOMPONENTSTATUS_HXX
#define GD_CDF_SUBCOMPONENTSTATUS_HXX

#ifdef WIN32
#include <windows.h>
#endif

#include "CDF_SubComponentStatus.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

// Dummy wrapper class for CDF_SubComponentStatus enum
class gd_CDF_SubComponentStatus : public RefCounted {
	GDCLASS(gd_CDF_SubComponentStatus, RefCounted);

public:
	enum Status {
		CDF_SCS_Consistent = ::CDF_SCS_Consistent,
		CDF_SCS_Unconsistent = ::CDF_SCS_Unconsistent,
		CDF_SCS_Stored = ::CDF_SCS_Stored,
		CDF_SCS_Modified = ::CDF_SCS_Modified
	};

protected:
	static void _bind_methods();
};

VARIANT_ENUM_CAST(gd_CDF_SubComponentStatus::Status);

#endif // GD_CDF_SUBCOMPONENTSTATUS_HXX