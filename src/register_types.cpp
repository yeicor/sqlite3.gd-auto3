#include "step_importer/register_types.h"
#ifdef OCGD_AUTO_BINDINGS_ENABLED
#include "gen_bindings/register_types.h"
#endif
#define OCGD_AI_BINDINGS_ENABLED # TODO: Set it elsewhere...
#ifdef OCGD_AI_BINDINGS_ENABLED
#include "ai_bindings/register_types.h"
#endif

extern "C" {
    // Initialization
    GDExtensionBool GDE_EXPORT build123d_gd_library_init(
        GDExtensionInterfaceGetProcAddress p_get_proc_address,
        GDExtensionClassLibraryPtr p_library,
        GDExtensionInitialization *r_initialization) {
        GDExtensionBool r = ocgd_step_importer_library_init(p_get_proc_address, p_library, r_initialization);
        if (!r) return r;
#ifdef OCGD_AUTO_BINDINGS_ENABLED
        r = ocgd_auto_library_init(p_get_proc_address, p_library, r_initialization);
        if (!r) return r;
#endif
#ifdef OCGD_AI_BINDINGS_ENABLED
        r =  ocgd_ai_library_init(p_get_proc_address, p_library, r_initialization);
        if (!r) return r;
#endif
return true;
    }
}
