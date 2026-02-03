/* godot-cpp integration binding for Storage_OpenMode.
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_Storage_OpenMode.hxx"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void gd_Storage_OpenMode::_bind_methods() {
	// Enum binding.
	BIND_ENUM_CONSTANT(VSNone);
	BIND_ENUM_CONSTANT(VSRead);
	BIND_ENUM_CONSTANT(VSWrite);
	BIND_ENUM_CONSTANT(VSReadWrite);
}

// No methods or underlying C++ object to implement, as this is an enum wrapper.