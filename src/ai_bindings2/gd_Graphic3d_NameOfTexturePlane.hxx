/* godot-cpp OpenCASCADE binding: gd_Graphic3d_NameOfTexturePlane.hxx
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef GD_GRAPHIC3D_NAMEOFTEXTUREPLANE_HXX
#define GD_GRAPHIC3D_NAMEOFTEXTUREPLANE_HXX

#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include "Graphic3d_NameOfTexturePlane.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

//! Type of the texture projection plane for both S and T texture coordinate.
/*!
 * This class wraps the Graphic3d_NameOfTexturePlane enum for Godot.
 */
class gd_Graphic3d_NameOfTexturePlane : public RefCounted {
	GDCLASS(gd_Graphic3d_NameOfTexturePlane, RefCounted);

public:
	//! Type of the texture projection plane for both S and T texture coordinate.
	enum NameOfTexturePlane {
		NOTP_XY = Graphic3d_NOTP_XY,
		NOTP_YZ = Graphic3d_NOTP_YZ,
		NOTP_ZX = Graphic3d_NOTP_ZX,
		NOTP_UNKNOWN = Graphic3d_NOTP_UNKNOWN
	};

protected:
	static void _bind_methods();

private:
	// No underlying C++ object pointer needed, as this is just an enum wrapper.
};

VARIANT_ENUM_CAST(gd_Graphic3d_NameOfTexturePlane::NameOfTexturePlane);

#endif // GD_GRAPHIC3D_NAMEOFTEXTUREPLANE_HXX