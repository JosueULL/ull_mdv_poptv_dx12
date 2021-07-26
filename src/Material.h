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
		if (TextureBinds.size() < rootIndex+1)
			TextureBinds.resize(rootIndex+1);
		TextureBinds[rootIndex] = tb;
	}

	std::vector<TextureBind> TextureBinds;

private:

};