#pragma once
#include "Math/Vector/Vector3.h"
#include "Math/Quaternion/Quaternion.h"
#include "Cell.h"

namespace VolumeRendering {
	class Volume {
	public:
		Volume() = default;
		Volume(int width, int height, int depth);
		~Volume();
		float cellWidth;
		float cellHeight;
		float cellDepth;
		void Resize(int width, int height, int depth);
		Cell* Data() { return cells; }
		const Cell* Data() const { return cells; }
		Cell& GetCell(int x, int y, int z);
		int CellCount() const { return width * height * depth; }
		mff::Vector3<float> position;
		mff::Quaternion<float> rotate;
		mff::Vector3<float> scale;
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		int GetDepth() const { return depth; }
	private:
		Volume(const Volume&) = delete;
		Volume& operator=(const Volume&) = delete;
		void DestroyCells();
		int width = 1;
		int height = 1;
		int depth = 1;
		Cell* cells = nullptr;
	};
} // namespace VolumeRendering