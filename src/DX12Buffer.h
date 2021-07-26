#pragma once

#include <d3d12.h>
#include "wrl.h"
#include <vector>
#include <string>

struct DX12Buffer {
	int size;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> slotData;
	
	static DX12Buffer Create(ID3D12Device* device, int slotCount, int size, void* data, std::string name);

	void Update(int slotIndex, const void* data);
};
