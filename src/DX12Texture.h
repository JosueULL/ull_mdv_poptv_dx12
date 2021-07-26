#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "Texture.h"

struct DX12Texture {
	Microsoft::WRL::ComPtr<ID3D12Resource>	image;
	Microsoft::WRL::ComPtr<ID3D12Resource>	uploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>  srvDescriptorHeap;

	static DX12Texture Create(ID3D12Device* device, const Texture &texture, ID3D12GraphicsCommandList* uploadCommandList, std::string name) {
		DX12Texture dx12texture;

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.NumDescriptors = 1;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NodeMask = 0;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&dx12texture.srvDescriptorHeap));

		int width = texture.width;
		int height = texture.height;

		static const auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, width, height, 1, 1);

		device->CreateCommittedResource(&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&dx12texture.image));

		static const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto uploadBufferSize = GetRequiredIntermediateSize(dx12texture.image.Get(), 0, 1);
		const auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		device->CreateCommittedResource(&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&dx12texture.uploadBuffer));

		D3D12_SUBRESOURCE_DATA srcData;
		srcData.pData = texture.data.data();
		srcData.RowPitch = width * 4;
		srcData.SlicePitch = width * height * 4;

		UpdateSubresources(uploadCommandList, dx12texture.image.Get(), dx12texture.uploadBuffer.Get(), 0, 0, 1, &srcData);
		const auto transition = CD3DX12_RESOURCE_BARRIER::Transition(dx12texture.image.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		uploadCommandList->ResourceBarrier(1, &transition);

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		device->CreateShaderResourceView(dx12texture.image.Get(), &shaderResourceViewDesc, dx12texture.srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


		std::string tName = "Texture ";
		tName.append(name);
		dx12texture.image->SetName(std::wstring(tName.begin(), tName.end()).c_str());

		return dx12texture;
	}
};