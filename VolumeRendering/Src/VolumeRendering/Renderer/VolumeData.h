#pragma once
#include "../Types.h"
namespace VolumeRendering {

	struct VolumeData {
		mat4 worldInverse;
		vec4 cellSize;
	};
} // namespace VolumeRendering