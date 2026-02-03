/* godot-cpp OpenCASCADE OSD_LoadMode binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_OSD_LoadMode.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//! This enumeration is used to load shareable libraries.
// No methods to bind; only enum wrapper.

void gd_OSD_LoadMode::_bind_methods() {
	// Bind enum constants for Godot.
	BIND_ENUM_CONSTANT(RTLD_LAZY);
	BIND_ENUM_CONSTANT(RTLD_NOW);
}