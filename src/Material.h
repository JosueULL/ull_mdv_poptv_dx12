#pragma once

#include <vector>
#include <string>

class Material {

public:
	Material() : 
		TextureBinds()
	{
	}

	struct TextureBind {
		std::string Id;
		uint32_t RootIndex;

		TextureBind() :
			Id(),
			RootIndex(9)
		{
		}
	};
	
	void SetTexture(uint32_t rootIndex, std::string resourceId) {
		TextureBind tb;
		tb.Id = resourceId;
		tb.RootIndex = rootIndex;
		int size = rootIndex + 1;
		if (TextureBinds.size() < size)
			TextureBinds.resize(size);
		TextureBinds[rootIndex] = tb;
	}

	void SetShader(std::string id) {
		shader_ = id;
	}

	std::string GetShader() {
		return shader_;
	}

	std::vector<TextureBind> TextureBinds;

private:
	std::string shader_;
};