#include <iostream>

#include "../Types.h"
#include "Renderer.h"
#include "CameraData.h"
#include "VolumeData.h"
#include "../Cell.h"
#include "../Volume.h"

namespace VolumeRendering {
	struct Vertex {
		mff::Vector3<float> position;
		mff::Vector4<float> color;
	};

	Vertex vertices[] = {
		{ { -1.0f, 1.0f,0.0f },{ 1.0f,1.0f,1.0f,1.0f } },
	{ { -1.0f,-1.0f,0.0f },{ 1.0f,1.0f,1.0f,1.0f }},
	{ { 1.0f,-1.0f,0.0f },{ 1.0f,1.0f,1.0f,1.0f }},
	{ { 1.0f, 1.0f,0.0f },{ 1.0f,1.0f,1.0f,1.0f }},
	};

	unsigned int indices[] = {
		0,1,2,2,3,0
	};

	const char vsCode[] = 
	"#version 450\n"
	"layout(location = 0) in vec3 inPosition;"
	"void main() {"
	"	gl_Position = vec4(inPosition, 1.0);"
	"}";
	const char fsCode[] = 
		"#version 450\n"
		"layout(std140, binding = 1) buffer CameraData {"
		"vec4 resolution;"
		"vec4 position;"
		"vec4 forward;"
		"vec4 up;"
		"} camera;"

		"struct Cell {"
		"vec4 data;"
		"};"

		"layout(std140, binding = 2) buffer VolumeData {"
		"mat4 worldInverse;"
		"vec4 cellSize;"
		"Cell cells[];"
		"};"

		"Cell GetCell(vec3 coord) {"
		"if (0 > coord.x || 1.0 < coord.x || 0 > coord.y || 1.0 < coord.y || 0 > coord.z || 1.0 < coord.z) {"
		"	Cell dummy;"
		"	dummy.data = vec4(0.0);"
		"	return dummy;"
		"}"
		"return cells[int(coord.x * cellSize.x) + int(coord.y * cellSize.y) * int(cellSize.x) + int(coord.z * cellSize.z) * int(cellSize.x * cellSize.y)];"
		"}"

		"Cell GetEncloseCell(vec3 worldPos) {"
		"vec3 localPos = (worldInverse * vec4(worldPos, 1.0)).xyz;"
		"localPos += vec3(1.0);"
		"localPos *= 0.5;"
		"return GetCell(localPos);"
		"}"

		"struct Ray {"
		"vec3 position;"
		"vec3 direction;"
		"};"

		"out vec4 fragColor;"

		"void main() {"
		"vec2 coord = (gl_FragCoord.xy * 2.0 - camera.resolution.xy) / (min(camera.resolution.x, camera.resolution.y));"

		"vec3 right = cross(camera.forward.xyz, camera.up.xyz);"
		"float depthTarget = camera.resolution.z;"
		"Ray ray;"
		"ray.position = camera.position.xyz;"
		"ray.direction = normalize(camera.forward.xyz * depthTarget + camera.up.xyz * coord.y + right * coord.x);"

		"int itelation = 1024;"
		"float length = 25.0;"
		"float perLen = length / itelation;"
		"float opacity = 0.0f;"
		"for (int i = 0; i < itelation; ++i) {"
		"	vec3 worldPos = ray.position + (i * perLen) * ray.direction;"
		"	vec4 res = GetEncloseCell(worldPos).data;"

		"	float a = res.a;"
		"	float o = (1 - opacity);"
		"	opacity += a * o;"
		"	res.rgb *= a;"
		"	fragColor.rgb += o * res.rgb;"
		"	/*"
		"	fragColor += res;"
		"	*/"
		"	if (opacity > 1.0) {"
		"		break;"
		"	}"
		"}"
		"fragColor.a = opacity;"
		"}";

	struct Camera {
		vec4 resolution;
		vec4 position;
		vec4 forward;
		vec4 up;
	};

	Renderer::~Renderer() {
		eg::Program::DeleteProgram(program);
	}

	bool Renderer::Init(int width, int height, const char * title, int cellCount) {
		if (!window.Init(width, height, title)) {
			return false;
		}
		if (!vertex.Init(sizeof(vertices), eg::BufferType_Vertex)) {
			return false;
		}

		if (!index.Init(sizeof(indices), eg::BufferType_Index)) {
			return false;
		}

		vertex.Update(vertices, sizeof(vertices));
		index.Update(indices, sizeof(indices));

		if (!vao.Init(&vertex, &index)) {
			return false;
		}

		vao.Bind();
		vao.SetAttribute(0, 3, eg::AttributeType_float, sizeof(Vertex), offsetof(Vertex, position));
		vao.SetAttribute(1, 4, eg::AttributeType_float, sizeof(Vertex), offsetof(Vertex, color));
		vao.UnBind();

		/*program = eg::Program::CreateProgram("Res/Shader/Simple.vert", "Res/Shader/Simple.frag");
		if (!program) {
			return false;
		}*/

		program = eg::Program::CreateProgramFromCode(vsCode, fsCode);
		if (!program) {
			return false;
		}
		if (!cameraData.Init(sizeof(Camera), eg::BufferType_Generic)) {
			return false;
		}
		cameraData.Bind(1);

		if (!volume.Init(sizeof(Cell) * cellCount + sizeof(VolumeData), eg::BufferType_Generic)) {
			return false;
		}
		volume.Bind(2);

		this->width = width;
		this->height = height;
		return true;
	}

	bool Renderer::ShouldClose() const {
		return window.ShouldClose();
	}

	void Renderer::Draw() {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		vao.Bind();
		program->UseProgram();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

		vao.UnBind();
		window.Swap();
		window.PollEvents();
	}

	int Renderer::GetWidth() const {
		return width;
	}

	int Renderer::GetHeight() const {
		return height;
	}

	void Renderer::SetVolumeData(const Volume & volume) {
		VolumeData volumeData;
		volumeData.cellSize = vec4(volume.GetWidth(), volume.GetHeight(), volume.GetDepth(), 0.0f);
		volumeData.worldInverse = mat4(1.0f);
		this->volume.Update(&volumeData, sizeof(VolumeData));
		this->volume.Update(volume.Data(), sizeof(Cell) * volume.CellCount(), sizeof(VolumeData));
	}
	

	void Renderer::SetCameraData(const CameraData & cameraData) {
		Camera cam;
		float depthTarget = 1.0f / (tanf(cameraData.fov * 0.5f));
		cam.resolution = vec4(static_cast<float>(width),static_cast<float>(height),depthTarget,0.0f);
		cam.position = vec4(cameraData.position, 0.0f);
		cam.forward = vec4(cameraData.quaternion.rotate(forward), 0.0f);
		cam.up = vec4(cameraData.quaternion.rotate(up), 0.0f);
		this->cameraData.Update(&cam, sizeof(Camera));
	}

	GLFWwindow * Renderer::GetWindowPtr() {
		return window.GetWindowPtr();
	}

} // namespace VolumeRendering
