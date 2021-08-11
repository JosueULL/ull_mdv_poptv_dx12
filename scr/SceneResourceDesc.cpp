#include "SceneResourcesDesc.h"

void SceneResourceDesc::ReleaseAll() {
	for (GraphicResourceDesc gd : Graphics) {
		if (gd.Type == GraphicResourceDesc::ResourceType::Mesh) {
			delete static_cast<Mesh*>(gd.Data);
		}

		gd.Data = nullptr;
	}

	Graphics.clear();
}