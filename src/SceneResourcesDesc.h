#pragma once

#include "GraphicResourceDesc.h"
#include <vector>

class SceneResourcesDesc {
public:
	std::vector<GraphicResourceDesc> Graphics;

	void ReleaseAll() {
		for (GraphicResourceDesc gd : Graphics) {
			if (gd.Type == GraphicResourceDesc::ResourceType::Mesh ||
				gd.Type == GraphicResourceDesc::ResourceType::Texture) {
				delete gd.Data;
				gd.Data = nullptr;
			}
		}
		Graphics.clear();
	}
};