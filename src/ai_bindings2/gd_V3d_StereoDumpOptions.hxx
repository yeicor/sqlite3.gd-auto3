/* godot-cpp OpenCASCADE V3d_StereoDumpOptions binding.
 *
 * This is free and unencumbered software released into the public domain.
 */

#ifndef GD_V3D_STEREODUMPOPTIONS_HXX
#define GD_V3D_STEREODUMPOPTIONS_HXX

// Platform-specific includes.
#ifdef WIN32
#include <windows.h>
#endif

#include "V3d_StereoDumpOptions.hxx"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

//! Options to be used with image dumping.
//! Notice that the value will have no effect with disabled stereo output.
class gd_V3d_StereoDumpOptions : public RefCounted {
	GDCLASS(gd_V3d_StereoDumpOptions, RefCounted);

public:
	enum StereoDumpOptions {
		SDO_MONO = V3d_SDO_MONO,        //!< ignore stereo mode and dump monographic projection for stereo camera
		SDO_LEFT_EYE = V3d_SDO_LEFT_EYE,//!< dump only left  eye projection for stereo camera
		SDO_RIGHT_EYE = V3d_SDO_RIGHT_EYE,//!< dump only right eye projection for stereo camera
		SDO_BLENDED = V3d_SDO_BLENDED   //!< dump blended pair specific to the active device output Graphic3d_StereoMode
		                                //!< (result will be undefined for modes like Graphic3d_StereoMode_QuadBuffer)
	};

protected:
	static void _bind_methods();

public:
	gd_V3d_StereoDumpOptions();
	~gd_V3d_StereoDumpOptions();

private:
	// No underlying C++ object pointer needed, as this is just an enum wrapper.
};

VARIANT_ENUM_CAST(gd_V3d_StereoDumpOptions::StereoDumpOptions);

#endif // GD_V3D_STEREODUMPOPTIONS_HXX