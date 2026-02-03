#include "register_types.h"
#include "ocgd_step_reader.h"
#include "ocgd_iges_reader.h"
#include "ocgd_brep_reader.h"
#include "ocgd_brep_writer.h"
#include "ocgd_step_writer.h"
#include "ocgd_iges_writer.h"
#include "ocgd_glb_exporter.h"
#include "ocgd_shape.h"
#include "ocgd_topology_explorer.h"
#include "ocgd_measurement_tool.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void ocgd_ai_bindings4_register_types(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    GDREGISTER_CLASS(ocgd_shape);
    GDREGISTER_CLASS(ocgd_step_reader);
    GDREGISTER_CLASS(ocgd_iges_reader);
    GDREGISTER_CLASS(ocgd_brep_reader);
    GDREGISTER_CLASS(ocgd_brep_writer);
    GDREGISTER_CLASS(ocgd_step_writer);
    GDREGISTER_CLASS(ocgd_iges_writer);
    GDREGISTER_CLASS(ocgd_glb_exporter);
    GDREGISTER_CLASS(ocgd_topology_explorer);
    GDREGISTER_CLASS(ocgd_measurement_tool);
}

void ocgd_ai_bindings4_unregister_types(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    // Nothing to do yet
}

// GDExtension entry point for ai_bindings4
extern "C" {
    GDExtensionBool GDE_EXPORT ocgd_ai_bindings4_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization) {
        
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        
        init_obj.register_initializer(ocgd_ai_bindings4_register_types);
        init_obj.register_terminator(ocgd_ai_bindings4_unregister_types);
        init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);
        
        return init_obj.init();
    }
}