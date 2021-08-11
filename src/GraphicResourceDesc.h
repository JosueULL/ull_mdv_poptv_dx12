#pragma once

#include "Common.h"
#include <string>


class GraphicResourceDesc {
public:
	std::string Id;
	void* Data;

	GraphicResourceDesc() :
		Id(""),
		Data(nullptr)
	{
	}

	template <class T>
	void Release() {
		T* data = (T*)Data;
		SAFE_FREE(data);
		Data = nullptr;
	};
};
