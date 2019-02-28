#pragma once
#include "../Types.h"
namespace VolumeRendering {
	struct CameraData {
		vec3 position;
		quat quaternion;
		float fov;
	};
} // namespace VolumeRendering