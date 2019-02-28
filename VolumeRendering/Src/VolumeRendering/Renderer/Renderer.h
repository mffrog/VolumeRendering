#pragma once
#include "Window/Window.h"
#include "Resource/Buffer.h"
#include "Resource/VAO.h"
#include "Resource/Shader.h"
#include "Resource/Texture.h"
namespace VolumeRendering {
	class Renderer {
	public:
		~Renderer();
		bool Init(int width, int height, const char* title, int cellCount);
		bool ShouldClose() const;
		void Draw();
		int GetWidth() const;
		int GetHeight() const;
		void SetVolumeData(const class Volume& volume);
		void SetCameraData(const struct CameraData& camera);
		GLFWwindow* GetWindowPtr();
	private:
		int width;
		int height;
		eg::Window window;
		eg::Buffer vertex;
		eg::Buffer index;
		eg::VAO vao;
		eg::Program* program;
		eg::Buffer cameraData;
		eg::Buffer volume;
	};
} // namespace VolumeRendering