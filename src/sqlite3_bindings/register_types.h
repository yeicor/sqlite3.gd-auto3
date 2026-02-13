#ifndef _sqlite3_register_types_H
#define _sqlite3_register_types_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

#include "SQLite3ResultSet.h"
#include "SQLite3Backup.h"

void sqlite3_gd_initialize_module(ModuleInitializationLevel p_level);

void sqlite3_gd_uninitialize_module(ModuleInitializationLevel p_level);

extern "C" {
    GDExtensionBool GDE_EXPORT sqlite3_gd_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization
    );
}

#endif // _sqlite3_register_types_H
