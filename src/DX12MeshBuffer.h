#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "Mesh.h"

struct DX12MeshBuffer 
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	int indexCount;

	static DX12MeshBuffer Create(ID3D12Device* device, const Mesh& mesh, ID3D12GraphicsCommandList* uploadCommandList) {
		DX12MeshBuffer meshBuffer;

		meshBuffer.indexCount = (UINT)mesh.indices.size();
		UINT sizeOfVertices = (UINT)mesh.vertices.size() * sizeof(Vertex);
		UINT sizeOfIndices = meshBuffer.indexCount * sizeof(UINT);
		const Vertex* vertices = &mesh.vertices[0];
		const int* indices = &mesh.indices[0];

		const int uploadBufferSize = sizeOfVertices + sizeOfIndices;
		const auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

		// Create upload buffer on CPU
		device->CreateCommittedResource(&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&meshBuffer.uploadBuffer));

		// Create vertex & index buffer on the GPU
		// HEAP_TYPE_DEFAULT is on GPU, we also initialize with COPY_DEST state
		// so we don't have to transition into this before copying into them
		const auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		const auto vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeOfVertices);
		device->CreateCommittedResource(&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&vertexBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&meshBuffer.vertexBuffer));

		const auto indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeOfIndices);
		device->CreateCommittedResource(&defaultHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&meshBuffer.indexBuffer));

		// Create buffer views
		meshBuffer.vertexBufferView.BufferLocation = meshBuffer.vertexBuffer->GetGPUVirtualAddress();
		meshBuffer.vertexBufferView.SizeInBytes = sizeOfVertices;
		meshBuffer.vertexBufferView.StrideInBytes = sizeof(Vertex);

		meshBuffer.indexBufferView.BufferLocation = meshBuffer.indexBuffer->GetGPUVirtualAddress();
		meshBuffer.indexBufferView.SizeInBytes = sizeOfIndices;
		meshBuffer.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

		// Copy data on CPU into the upload buffer
		void* p;
		meshBuffer.uploadBuffer->Map(0, nullptr, &p);
		::memcpy(p, vertices, sizeOfVertices);
		::memcpy(static_cast<unsigned char*>(p) + sizeOfVertices, indices, sizeOfIndices);
		meshBuffer.uploadBuffer->Unmap(0, nullptr);

		// Copy data from upload buffer on CPU into the index/vertex buffer on 
		// the GPU (upload->default)
		uploadCommandList->CopyBufferRegion(meshBuffer.vertexBuffer.Get(), 0, meshBuffer.uploadBuffer.Get(), 0, sizeOfVertices);
		uploadCommandList->CopyBufferRegion(meshBuffer.indexBuffer.Get(), 0, meshBuffer.uploadBuffer.Get(), sizeOfVertices, sizeOfIndices);

		// Barriers, batch them together
		const CD3DX12_RESOURCE_BARRIER barriers[2] = {
			CD3DX12_RESOURCE_BARRIER::Transition(meshBuffer.vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(meshBuffer.indexBuffer.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER)
		};

		uploadCommandList->ResourceBarrier(2, barriers);

		return meshBuffer;
	}
};