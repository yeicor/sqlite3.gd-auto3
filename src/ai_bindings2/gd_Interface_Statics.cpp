/* godot-cpp Open CASCADE Interface_Statics binding implementation.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * This file provides Godot GDExtension bindings for the Open CASCADE Interface_Statics macros.
 * See original documentation in Interface_Statics.hxx for usage and initialization details.
 */

#include "gd_Interface_Statics.hxx"

// Platform-specific includes
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/core/class_db.hpp>
#include <Standard_Failure.hxx>

// NOTE: The original Interface_Statics.hxx only defines macros for static handle management.
// There are no classes marked with Standard_EXPORT or similar macros, nor any callable methods
// from Godot other than macro-based handle management. Therefore, no wrapper classes or method
// implementations are generated for this header, as per binding rules.

// If in the future Interface_Statics.hxx introduces Standard_EXPORT classes with callable methods,
// implementations should be added here following the binding conventions.