#pragma once

#include <d3d12.h>
#include "wrl.h"
#include <vector>

struct DX12ConstantBuffer {
	int size;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> slotData;

	static DX12ConstantBuffer Create(ID3D12Device* device, int slotCount, int size) {

		DX12ConstantBuffer cBuffer;
		cBuffer.size = size;
		cBuffer.slotData.resize(slotCount);

		for (int i = 0; i < slotCount; ++i) {
			const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			const auto constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

			cBuffer.slotData[i] = nullptr;

			// These will remain in upload heap because we use them only once per
			// frame.
			device->CreateCommittedResource(&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&constantBufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&cBuffer.slotData[i]));
		}

		return cBuffer;
	}

	void Update(int slotIndex, const void* data)
	{
		void* p;
		slotData[slotIndex]->Map(0, nullptr, &p);
		memcpy(p, data, size);
		slotData[slotIndex]->Unmap(0, nullptr);
	}
};
