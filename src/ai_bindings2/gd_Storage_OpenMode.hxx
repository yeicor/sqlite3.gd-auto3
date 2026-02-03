#ifndef GD_STORAGE_OPENMODE_HXX
#define GD_STORAGE_OPENMODE_HXX

/* godot-cpp integration binding for Storage_OpenMode.
 *
 * This is free and unencumbered software released into the public domain.
 */

//! Specifies opening modes for a file:
//! -   Storage_VSNone : no mode is specified
//! -   Storage_VSRead : the file is open for  reading operations
//! -   Storage_VSWrite : the file is open for writing operations
//! -   Storage_VSReadWrite : the file is open
//! for both reading and writing operations.

#include "Storage_OpenMode.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class gd_Storage_OpenMode : public RefCounted {
	GDCLASS(gd_Storage_OpenMode, RefCounted);

public:
	//! Specifies opening modes for a file.
	enum OpenMode {
		VSNone = Storage_VSNone,
		VSRead = Storage_VSRead,
		VSWrite = Storage_VSWrite,
		VSReadWrite = Storage_VSReadWrite
	};

protected:
	static void _bind_methods();

private:
	// No underlying C++ object pointer needed, as this is just an enum wrapper.
};

VARIANT_ENUM_CAST(gd_Storage_OpenMode::OpenMode);

#endif // GD_STORAGE_OPENMODE_HXX