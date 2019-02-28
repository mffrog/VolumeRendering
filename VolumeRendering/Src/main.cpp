#include <iostream>
#include "VolumeRendering/Renderer/Renderer.h"
#include "VolumeRendering/Renderer/CameraData.h"

#include "VolumeRendering/Cell.h"
#include "VolumeRendering/Volume.h"

using namespace VolumeRendering;
#include <vector>

struct ViewProjectionBuffer {
	mff::Matrix4x4<float> view;
	mff::Matrix4x4<float> projection;
	mff::Matrix4x4<float> vp;
};

void nothing() {}

vec3 right(1.0f, 0.0f, 0.0f);
float fov = 60.0f * static_cast<float>(M_PI) / 180.0f;


#define KeyPressed(key) glfwGetKey(renderer.GetWindowPtr(), (key) ) != GLFW_RELEASE

int main(const int argc, const char* argv[]) {
	int winWidth = 800;
	int winHeight = 600;

	//Scene
	float alpha = 1.0f / 16.0f;
	int size = 10;
	Volume scene(size, size, size);
	scene.scale = vec3(scene.GetWidth(), scene.GetHeight(), scene.GetDepth());
	//scene.scale *= 0.1f;
	for (int x = 0; x < scene.GetWidth(); ++x) {
		for (int y = 0; y < scene.GetHeight(); ++y) {
			for (int z = 0; z < scene.GetDepth(); ++z) {
				auto& cell = scene.GetCell(x, y, z);
				cell.data = mff::Vector4<float>((x + 1) / static_cast<float>(scene.GetWidth()), (y + 1) / static_cast<float>(scene.GetHeight()), (z + 1) / static_cast<float>(scene.GetDepth()), alpha);
		
				cell.data.a = alpha;
				
			}
		}
	}

	Renderer renderer;
	if (!renderer.Init(winWidth, winHeight, "VolumeRendering", scene.GetWidth() * scene.GetHeight() * scene.GetDepth())) {
		return 1;
	}

	vec3 cameraPos(0.0f, 0.0f, 1.0f);
	mff::Quaternion<float> cameraQuat;
	float speed = .2f;
	mff::Vector2<double> beforeCursorPos;
	bool drawCube = false;

	while (!renderer.ShouldClose()) {
		//カメラ移動
		if (KeyPressed(GLFW_KEY_W)) {
			cameraPos += cameraQuat.rotate(forward) * speed;
		}
		else if (KeyPressed(GLFW_KEY_S)) {
			cameraPos -= cameraQuat.rotate(forward) * speed;
		}

		if (KeyPressed(GLFW_KEY_E)) {
			cameraPos += up * speed;
		}
		else if (KeyPressed(GLFW_KEY_Q)) {
			cameraPos -= up * speed;
		}
		if (KeyPressed(GLFW_KEY_D)) {
			cameraPos += cameraQuat.rotate(right) * speed;
		}
		else if (KeyPressed(GLFW_KEY_A)) {
			cameraPos -= cameraQuat.rotate(right) * speed;
		}


		//カメラ回転
		static bool init = false;
		mff::Vector2<double> cursorPos;
		glfwGetCursorPos(renderer.GetWindowPtr(), &cursorPos.x, &cursorPos.y);
			if (!init) {
				beforeCursorPos = cursorPos;
				init = true;
			}
			else {
				auto delta = cursorPos - beforeCursorPos;
				delta.x /= winWidth;
				delta.y /= winHeight;
				float rotSpeed = 45.0f /(winHeight * 0.5f);
				rotSpeed /= 180.0f * static_cast<float>(M_PI);
				cameraQuat = cameraQuat * mff::MakeQuaternion(vec3(0.0f, 1.0f, 0.0f), static_cast<float>(-delta.x));
				beforeCursorPos = cursorPos;
			}

			//カーソル位置リセット
		if (KeyPressed(GLFW_KEY_SPACE)) {
			glfwSetCursorPos(renderer.GetWindowPtr(), winWidth * 0.5f, winHeight * 0.5f);
			beforeCursorPos.x = winWidth * 0.5f;
			beforeCursorPos.y = winHeight * 0.5f;
		}

		static bool accept = true;
		if (KeyPressed(GLFW_KEY_ENTER)) {
			if (accept) {
				drawCube = !drawCube;
			}
			accept = false;

		}
		else {
			accept = true;
		}

		static float time = 0.0f;
		time += 1.0f;
		float rad = time / 180.0f * static_cast<float>(M_PI);
		auto red = [](float angle)->float{
			if (angle <= 60.0f) {
				return 1.0f;
			}
			else if (300.0f <= angle) {
				return 1.0f;
			}
			else if (60.0f < angle && angle < 120.0f) {
				return 1.0f - (angle - 60.0f) / 60.0f;
			}
			else if(120.0f <= angle && angle <= 240.0f){
				return 0.0f;
			}
			else if(240.0f < angle && angle < 300.0f) {
				return (angle - 240.0f) / 60.0f;
			}
			return 0.0f;
		};
		auto green = [](float angle)->float{
			if (0.0f < angle && angle < 60.0f) {
				return angle / 60.0f;
			}
			else if (60.0f <= angle && angle <= 180.0f) {
				return 1.0f;
			}
			else if (180.0f < angle && angle < 240.0f) {
				return 1.0f - (angle - 180.0f) / 60.0f;
			}
			else if (240.0f <= angle && angle <= 360.0f) {
				return 0.0f;
			}
			return 0.0f;
		};
		auto blue = [](float angle)->float{
			if (0.0f <= angle && angle <= 120.0f) {
				return 0.0f;
			}
			else if (120.0f < angle && angle < 180.0f) {
				return (angle - 120.0f) / 60.0f;
			}
			else if (180.0f <= angle && angle <= 300.0f) {
				return 1.0f;
			}
			else if(300.0f < angle && angle < 360.0f) {
				return 1.0f - (angle - 300.0f) / 60.0f;
			}
			return 0.0f;
		};
		auto normalizer = [](float v) {
			return (v + 1.0f) * 0.5f;
		};
		if (time >= 360.0f) {
			time -= 360.0f;
		}
		vec4 color = vec4(red(time),green(time), blue(time), alpha);
		for (int x = 0; x < scene.GetWidth(); ++x) {
			for (int y = 0; y < scene.GetHeight(); ++y) {
				for (int z = 0; z < scene.GetDepth(); ++z) {
					auto& cell = scene.GetCell(x, y, z);
					cell.data = color;
				}
			}
		}
		
		CameraData camera;
		camera.fov = fov;
		camera.position = cameraPos;
		camera.quaternion = cameraQuat;
		renderer.SetCameraData(camera);

		renderer.SetVolumeData(scene);

		renderer.Draw();
	}

	return 0;
}