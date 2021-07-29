#pragma once

#include <string>

class GraphicResourceDesc {
public:

	enum class ResourceType {
		Mesh,
		Texture,
		RenderState,
		Material,
		ConstantBuffer,
		InstanceBuffer,
	};

	std::string Id;
	ResourceType Type;
	void* Data;
};

