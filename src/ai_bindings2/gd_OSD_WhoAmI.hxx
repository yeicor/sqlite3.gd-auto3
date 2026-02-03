/* godot-cpp OpenCASCADE OSD_WhoAmI binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef GD_OSD_WHOAMI_HXX
#define GD_OSD_WHOAMI_HXX

// Platform-specific includes.
#ifdef WIN32
#include <windows.h>
#endif

#include "OSD_WhoAmI.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

//! Allows great accuracy for error management.
//! This is private.
//! Binding for OSD_WhoAmI enum.
class gd_OSD_WhoAmIEnum : public RefCounted {
	GDCLASS(gd_OSD_WhoAmIEnum, RefCounted);

public:
	enum WhoAmI {
		Directory = OSD_WDirectory,
		DirectoryIterator = OSD_WDirectoryIterator,
		Environment = OSD_WEnvironment,
		File = OSD_WFile,
		FileNode = OSD_WFileNode,
		FileIterator = OSD_WFileIterator,
		Path = OSD_WPath,
		Process = OSD_WProcess,
		Protection = OSD_WProtection,
		Host = OSD_WHost,
		Disk = OSD_WDisk,
		Chronometer = OSD_WChronometer,
		Timer = OSD_WTimer,
		Package = OSD_WPackage,
		EnvironmentIterator = OSD_WEnvironmentIterator
	};

protected:
	static void _bind_methods() {}
};

VARIANT_ENUM_CAST(gd_OSD_WhoAmIEnum::WhoAmI);

#endif // GD_OSD_WHOAMI_HXX