#pragma once
#include "Math/Vector/Vector2.h"
#include "Math/Vector/Vector3.h"
#include "Math/Vector/Vector4.h"
#include "Math/Matrix/Matrix4x4.h"
#include "Math/Quaternion/Quaternion.h"
namespace VolumeRendering {
	using vec2 = mff::Vector2<float>;
	using vec3 = mff::Vector3<float>;
	using vec4 = mff::Vector4<float>;
	using mat4 = mff::Matrix4x4<float>;
	using quat = mff::Quaternion<float>;

	const vec3 forward = vec3(0.0f, 0.0f, -1.0f);
	const vec3 up = vec3(0.0f, 1.0f, 0.0f);
} // namespace VolumeRendering