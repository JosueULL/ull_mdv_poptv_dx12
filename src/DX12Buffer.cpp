#include "DX12Buffer.h"
#include "d3dx12.h"

DX12Buffer DX12Buffer::Create(ID3D12Device* device, int slotCount, int size, void* data, std::string name) {

	DX12Buffer cBuffer;
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

		std::string slotName = name;
		slotName.append(std::to_string(i));

		cBuffer.slotData[i]->SetName(std::wstring(slotName.begin(), slotName.end()).c_str());

		cBuffer.Update(i, data);
	}

	return cBuffer;
}

void DX12Buffer::Update(int slotIndex, const void* data)
{
	void* p;
	slotData[slotIndex]->Map(0, nullptr, &p);
	memcpy(p, data, size);
	slotData[slotIndex]->Unmap(0, nullptr);
}