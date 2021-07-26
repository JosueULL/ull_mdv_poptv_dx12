#pragma once

#include <string>

class GraphicResourceDesc {
public:

	enum class ResourceType {
		Mesh,
		Texture,
		ConstantBuffer,
		InstanceBuffer,
	};

	std::string Id;
	ResourceType Type;
	void* Data;
};

