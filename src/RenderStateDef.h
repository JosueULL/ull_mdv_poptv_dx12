#pragma once

#include <string>

struct RenderStateDef {
	std::string shaderPath;
	bool instancing;
	bool compileShader;
};
