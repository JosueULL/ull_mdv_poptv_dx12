#pragma once

#include "GraphicResourceDesc.h"
#include <vector>

class SceneResourcesDesc {
public:
	std::vector<GraphicResourceDesc> ConstantBuffers;
	std::vector<GraphicResourceDesc> InstanceBuffers;
	std::vector<GraphicResourceDesc> Textures;
	std::vector<GraphicResourceDesc> Materials;
	std::vector<GraphicResourceDesc> Meshes;
	std::vector<GraphicResourceDesc> RenderStates;

	void ReleaseAll();

private:
	template <class T>
	void Release(std::vector<GraphicResourceDesc>& resources) {
		for (auto& grb : resources) {
			grb.Release<T>();
		}
		resources.clear();
	}
};