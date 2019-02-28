#include "Volume.h"
namespace VolumeRendering {
	Volume::Volume(int width, int height, int depth) {
		Resize(width, height, depth);
	}

	Volume::~Volume() {
		DestroyCells();
	}

	void Volume::Resize(int width, int height, int depth) {
		DestroyCells();
		cells = new Cell[width * height * depth];
		this->width = width;
		this->height = height;
		this->depth = depth;
	}

	Cell & Volume::GetCell(int x, int y, int z) {
		if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
			static Cell dummy;
			return dummy;
		}
		return cells[x + y * width + z * width * height];
	}

	void Volume::DestroyCells() {
		if (cells) {
			delete[] cells;
		}
		cells = nullptr;
	}

} // namespace VolumeRendering