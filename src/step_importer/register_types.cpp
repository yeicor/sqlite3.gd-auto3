#include "step_iges_brep_importer.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_gdextension_types_step_importer([[maybe_unused]] ModuleInitializationLevel p_level) {
    if (p_level < MODULE_INITIALIZATION_LEVEL_SCENE) return; // Only initialize at the SCENE level or higher.
    GDREGISTER_CLASS(StepIgesBRepImporter);
}

void uninitialize_gdextension_types_step_importer([[maybe_unused]] ModuleInitializationLevel p_level) {
}

GDExtensionBool ocgd_step_importer_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                       GDExtensionClassLibraryPtr p_library,
                                                       GDExtensionInitialization *r_initialization) {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
    init_obj.register_initializer(initialize_gdextension_types_step_importer);
    init_obj.register_terminator(uninitialize_gdextension_types_step_importer);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
