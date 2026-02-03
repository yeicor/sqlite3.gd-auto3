/* godot-cpp OpenCASCADE AIS_NavigationMode binding implementation.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_AIS_NavigationMode.hxx"

#include <godot_cpp/core/class_db.hpp>
#include <Standard_Failure.hxx>

using namespace godot;

#ifdef WIN32
#include <windows.h>
#endif

void gd_AIS_NavigationMode::_bind_methods() {
	// Only enum is exposed; no methods to bind.
	// Bind enum constants for Godot.
	BIND_ENUM_CONSTANT(Orbit);
	BIND_ENUM_CONSTANT(FirstPersonFlight);
	BIND_ENUM_CONSTANT(FirstPersonWalk);

	BIND_CONSTANT(Lower);
	BIND_CONSTANT(Upper);
}