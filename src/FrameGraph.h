#pragma once

#include <vector>
#include <string>

#include "Material.h"


class Camera;

struct FrameGraphCBuffer {
	std::string Id;
	void* Data;
	uint32_t RootIndex;
};

struct FrameGraphMesh {
	std::string Id;
	FrameGraphCBuffer CBuffer;
	std::vector<Material::TextureBind> TextureBinds;
};

struct FrameGraphCamera {
	FrameGraphCBuffer CBuffer;
};

class FrameGraph {
public:
	FrameGraphCamera RenderCam;
	std::vector<FrameGraphMesh> Meshes;

	void Clear() {
		Meshes.clear();
	}
};