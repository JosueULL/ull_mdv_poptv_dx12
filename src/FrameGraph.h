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
	std::string RenderState;
};

struct FrameGraphCamera {
	FrameGraphCBuffer CBuffer;
};

class FrameGraph {
public:
	FrameGraphCamera RenderCam;
	FrameGraphCamera UICam;
	std::vector<FrameGraphMesh> Meshes;
	std::vector<FrameGraphMesh> InstancedMeshes;
	std::vector<FrameGraphMesh> UI;

	void Clear() {
		Meshes.clear();
		InstancedMeshes.clear();
		UI.clear();
	}
};