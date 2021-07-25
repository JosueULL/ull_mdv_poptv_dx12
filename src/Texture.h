#pragma once


#include <vector>

class Texture {
public:
	std::vector<std::uint8_t> data;
	int width;
	int height;

	Texture() :
		data(),
		width(1),
		height(1)
	{

	}
};
