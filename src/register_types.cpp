#include "register_types.h"

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>

#include "SQLite3Binding.h"
#include "SQLite3Database.h"
// Include other classes as they are created
#include "SQLite3Statement.h"
#include "SQLite3ResultSet.h"
#include "SQLite3Backup.h"
#include "SQLite3Blob.h"

using namespace godot;

void gdext_initialize_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Register wrapped classes
    GDREGISTER_CLASS(SQLite3);
    GDREGISTER_CLASS(SQLite3Database);
    GDREGISTER_CLASS(SQLite3Statement);
    GDREGISTER_CLASS(SQLite3ResultSet);
    GDREGISTER_CLASS(SQLite3Backup);
    GDREGISTER_CLASS(SQLite3Blob);
}

void gdext_uninitialize_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    // Teardown logic (if any) goes here.
}

extern "C" {
    GDExtensionBool GDE_EXPORT gdext_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        const GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization
    ) {
        godot::GDExtensionBinding::InitObject init_obj(
            p_get_proc_address, p_library, r_initialization
        );

        init_obj.register_initializer(gdext_initialize_module);
        init_obj.register_terminator(gdext_uninitialize_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
