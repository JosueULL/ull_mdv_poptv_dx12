#pragma once

#include <string>

class GraphicResourceDesc {
public:

	enum class ResourceType {
		Mesh,
		Texture,
		ConstantBuffer
	};

	std::string Id;
	ResourceType Type;
	void* Data;
};

