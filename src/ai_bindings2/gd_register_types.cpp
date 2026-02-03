/* godot-cpp Open CASCADE binding: gd_register_types.cpp
 *
 * This is free and unencumbered software released into the public domain.
 */

#include "gd_register_types.hxx"

#include <gdextension_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "gd_Graphic3d_NameOfTexturePlane.hxx"
#include "gd_V3d_StereoDumpOptions.hxx"
#include "gd_AIS_NavigationMode.hxx"
#include "gd_CDF_SubComponentStatus.hxx"
#include "gd_TopOpeBRepDS_Config.hxx"
#include "gd_Vrml_MaterialBindingAndNormalBinding.hxx"
#include "gd_Aspect_XRActionType.hxx"
#include "gd_Storage_OpenMode.hxx"
#include "gd_OSD_LoadMode.hxx"
#include "gd_OSD_WhoAmI.hxx"
#include "gd_Interface_Statics.hxx"
#include "gd_Standard_WarningsDisable.hxx"
#include "gd_AIS_SelectionScheme.hxx"
#include "gd_TDF_LabelNodePtr.hxx"

using namespace godot;

void initialize_gd_occt_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(gd_Graphic3d_NameOfTexturePlane);
	GDREGISTER_CLASS(gd_V3d_StereoDumpOptions);
	GDREGISTER_CLASS(gd_AIS_NavigationMode);
	GDREGISTER_CLASS(gd_CDF_SubComponentStatus);
	GDREGISTER_CLASS(gd_TopOpeBRepDS_Config);
	GDREGISTER_CLASS(gd_Vrml_MaterialBindingAndNormalBinding);
	GDREGISTER_CLASS(gd_Aspect_XRActionType);
	GDREGISTER_CLASS(gd_Storage_OpenMode);
	GDREGISTER_CLASS(gd_OSD_LoadMode);
	GDREGISTER_CLASS(gd_OSD_WhoAmIEnum);
	GDREGISTER_CLASS(gd_AIS_SelectionScheme);

	// No GDREGISTER_CLASS for gd_TDF_LabelNodePtr, as no wrapper class was generated.
}

void uninitialize_gd_occt_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT gd_occt_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_gd_occt_module);
	init_obj.register_terminator(uninitialize_gd_occt_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}