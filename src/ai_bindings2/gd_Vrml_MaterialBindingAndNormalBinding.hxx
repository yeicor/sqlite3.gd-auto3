/* godot-cpp OpenCASCADE GDExtension binding.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * This file is a binding wrapper for Vrml_MaterialBindingAndNormalBinding.hxx.
 * See original copyright and license in the Open CASCADE distribution.
 *
 * This file provides Godot GDExtension bindings for the Vrml_MaterialBindingAndNormalBinding enum.
 */

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#ifndef GD_VRML_MATERIALBINDINGANDNORMALBINDING_HXX
#define GD_VRML_MATERIALBINDINGANDNORMALBINDING_HXX

#include "Vrml_MaterialBindingAndNormalBinding.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

// Dummy wrapper class for Vrml_MaterialBindingAndNormalBinding enum.
// This is required to expose the enum to Godot via GDExtension.
class gd_Vrml_MaterialBindingAndNormalBinding : public RefCounted {
	GDCLASS(gd_Vrml_MaterialBindingAndNormalBinding, RefCounted);

public:
	// Enum mapping from Vrml_MaterialBindingAndNormalBinding
	enum MaterialBindingAndNormalBinding {
		DEFAULT = Vrml_DEFAULT,
		OVERALL = Vrml_OVERALL,
		PER_PART = Vrml_PER_PART,
		PER_PART_INDEXED = Vrml_PER_PART_INDEXED,
		PER_FACE = Vrml_PER_FACE,
		PER_FACE_INDEXED = Vrml_PER_FACE_INDEXED,
		PER_VERTEX = Vrml_PER_VERTEX,
		PER_VERTEX_INDEXED = Vrml_PER_VERTEX_INDEXED
	};

protected:
	static void _bind_methods();

};

VARIANT_ENUM_CAST(gd_Vrml_MaterialBindingAndNormalBinding::MaterialBindingAndNormalBinding);

#endif // GD_VRML_MATERIALBINDINGANDNORMALBINDING_HXX