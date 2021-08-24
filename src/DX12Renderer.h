//
// Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef ANTERU_D3D12_SAMPLE_D3D12SAMPLE_H_
#define ANTERU_D3D12_SAMPLE_D3D12SAMPLE_H_

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "BackEndRenderer.h"
#include "DX12Buffer.h"
#include "DX12RenderState.h"

class Mesh;
class Texture;
class SceneResourcesDesc;
class FrameGraph;

struct FrameGraphCamera;
struct FrameGraphMesh;
struct FrameGraphCBuffer;

struct DX12MeshBuffer;
struct DX12Texture;
struct DX12InstanceBuffer;


class Window;

///////////////////////////////////////////////////////////////////////////////
class DX12Renderer : public BackEndRenderer
{
public:
	DX12Renderer (const DX12Renderer&) = delete;
	DX12Renderer& operator= (const DX12Renderer&) = delete;

	DX12Renderer ();
	~DX12Renderer ();

	virtual void Render(const FrameGraph* frameGraph);
	virtual void Initialize(const Window& window);
	virtual void Shutdown();
	virtual void LoadResources(const SceneResourcesDesc& sceneRes);

protected:
	int GetQueueSlot () const
	{
		return currentBackBuffer_;
	}

	static const int QUEUE_SLOT_COUNT = 3;

	static constexpr int GetQueueSlotCount ()
	{
		return QUEUE_SLOT_COUNT;
	}

	D3D12_VIEWPORT viewport_;
	D3D12_RECT rectScissor_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets_ [QUEUE_SLOT_COUNT];
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetDepthStencil_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

	HANDLE frameFenceEvents_ [QUEUE_SLOT_COUNT];
	Microsoft::WRL::ComPtr<ID3D12Fence> frameFences_ [QUEUE_SLOT_COUNT];
	UINT64 currentFenceValue_;
	UINT64 fenceValues_[QUEUE_SLOT_COUNT];

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthStencilDescriptorHeap_;

private:

	struct SharedBuffer {
		float elapsedTime;
		float sinTime;
		float pad2;
		float pad3;
	};
	
	void PrepareRender ();
	void FinalizeRender ();
	void Present ();

	void CreateDeviceAndSwapChain (const Window& window);
	void CreateAllocatorsAndCommandLists ();
	void CreateViewportScissor (const Window& window);
	void CreateDefaultRenderStates();
	
	void SetupSwapChain ();
	void SetupRenderTargets ();

	void UpdateSharedBuffer(ID3D12GraphicsCommandList* commandList);
	void UpdateCBuffer(const FrameGraphCBuffer& cbuffer, ID3D12GraphicsCommandList* commandList);
	void SetRenderState(std::string renderStateId, ID3D12GraphicsCommandList* commandList);

	void DrawMesh(std::string id, ID3D12GraphicsCommandList* commandList, int instanceCount);
	void DrawMeshes(const FrameGraphCamera& cam, const std::vector<FrameGraphMesh>& meshes, ID3D12GraphicsCommandList* commandList);
	void DrawInstancedMeshes(const FrameGraph* frameGraph, ID3D12GraphicsCommandList* commandList);

	const Window* window_;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators_[QUEUE_SLOT_COUNT];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandLists_[QUEUE_SLOT_COUNT];

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignatureInstancing_;

	int currentBackBuffer_ = 0;
	
	std::int32_t renderTargetViewDescriptorSize_;

	std::unordered_map<std::string, DX12MeshBuffer> resMeshBuffers_;
	std::unordered_map<std::string, DX12Texture> resTextures_;
	std::unordered_map<std::string, DX12Buffer> resConstantBuffers_;
	std::unordered_map<std::string, DX12InstanceBuffer> resInstanceBuffers_;
	std::unordered_map<std::string, DX12RenderState*> resRenderStates_;
	DX12RenderState* currentRenderState_;
	DX12Buffer sharedBuffer;
	SharedBuffer sharedBufferData;
};

#endif
