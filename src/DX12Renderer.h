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

class Mesh;
class Texture;
class GraphicResourceDesc;
class FrameGraph;

struct RendererCommand;
struct FrameGraphCBuffer;

struct DX12MeshBuffer;
struct DX12Texture;
struct DX12Buffer;
struct DX12InstanceBuffer;
struct DX12RenderState;

class Window;

///////////////////////////////////////////////////////////////////////////////
class DX12Renderer
{
public:
	DX12Renderer (const DX12Renderer&) = delete;
	DX12Renderer& operator= (const DX12Renderer&) = delete;

	DX12Renderer ();
	virtual ~DX12Renderer ();

	void Render(std::vector<RendererCommand> commands);
	void Render(const FrameGraph* frameGraph);
	void Initialize(const Window& window);
	void Shutdown();

	void LoadResources(std::vector<GraphicResourceDesc>);

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

	
	virtual void InitializeImpl (ID3D12GraphicsCommandList* uploadCommandList);
	
private:
	
	void PrepareRender ();
	void FinalizeRender ();
	void Present ();

	void CreateDeviceAndSwapChain (const Window& window);
	void CreateAllocatorsAndCommandLists ();
	void CreateViewportScissor (const Window& window);
	void CreateRootSignature();
	void CreateRootSignature2();

	void SetupSwapChain ();
	void SetupRenderTargets ();

	void ProcessRenderCommand(const RendererCommand& cmd, ID3D12GraphicsCommandList* commandList);
	void UpdateCBuffer(const FrameGraphCBuffer& cbuffer, ID3D12GraphicsCommandList* commandList);

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
	std::unordered_map<std::string, DX12RenderState> resRenderStates_;
};

#endif
