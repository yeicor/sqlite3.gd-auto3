/* godot-cpp OpenCASCADE binding: gd_CDF_SubComponentStatus.cpp
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Implementation for gd_CDF_SubComponentStatus.hxx
 */

#include "gd_CDF_SubComponentStatus.hxx"

#include <godot_cpp/core/class_db.hpp>

#ifdef WIN32
#include <windows.h>
#endif

using namespace godot;

void gd_CDF_SubComponentStatus::_bind_methods() {
	BIND_ENUM_CONSTANT(CDF_SCS_Consistent);
	BIND_ENUM_CONSTANT(CDF_SCS_Unconsistent);
	BIND_ENUM_CONSTANT(CDF_SCS_Stored);
	BIND_ENUM_CONSTANT(CDF_SCS_Modified);
}