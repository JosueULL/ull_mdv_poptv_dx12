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

#include "DX12Renderer.h"

#include <dxgi1_4.h>
#include "d3dx12.h"
#include <iostream>
#include <algorithm>
#include <combaseapi.h>

#include "DX12MeshBuffer.h"
#include "DX12Texture.h"
#include "DX12Buffer.h"
#include "DX12InstanceBuffer.h"
#include "DX12RenderState.h"

#include "ImageIO.h"
#include "Window.h"
#include "Mesh.h"
#include "Texture.h"
#include "ConstantBufferDef.h"
#include "InstanceBufferDef.h"
#include "RenderStateDef.h"
#include "FrameGraph.h"
#include "GraphicResourceDesc.h"

#ifdef max 
#undef max
#endif

#define RENDERSTATE_DEFAULT "Assets/Shaders/baseTexture.hlsl"
#define RENDERSTATE_DEFAULT_INSTANCED "Assets/Shaders/instancedTexture.hlsl"

using namespace Microsoft::WRL;

namespace {
struct RenderEnvironment
{
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12CommandQueue> queue;
	ComPtr<IDXGISwapChain> swapChain;
};

///////////////////////////////////////////////////////////////////////////////
/**
Create everything we need for rendering, this includes a device, a command queue,
and a swap chain.
*/
RenderEnvironment CreateDeviceAndSwapChainHelper (
	_In_opt_ IDXGIAdapter* adapter,
	D3D_FEATURE_LEVEL minimumFeatureLevel,
	_In_ const DXGI_SWAP_CHAIN_DESC* swapChainDesc)
{
	RenderEnvironment result;

	auto hr = D3D12CreateDevice (adapter, minimumFeatureLevel,
		IID_PPV_ARGS (&result.device));

	if (FAILED (hr)) {
		throw std::runtime_error ("Device creation failed.");
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	hr = result.device->CreateCommandQueue (&queueDesc, IID_PPV_ARGS (&result.queue));

	if (FAILED (hr)) {
		throw std::runtime_error ("Command queue creation failed.");
	}

	ComPtr<IDXGIFactory4> dxgiFactory;
	hr = CreateDXGIFactory1 (IID_PPV_ARGS (&dxgiFactory));

	if (FAILED (hr)) {
		throw std::runtime_error ("DXGI factory creation failed.");
	}

	// Must copy into non-const space
	DXGI_SWAP_CHAIN_DESC swapChainDescCopy = *swapChainDesc;
	hr = dxgiFactory->CreateSwapChain (
		result.queue.Get (),
		&swapChainDescCopy,
		&result.swapChain
		);

	if (FAILED (hr)) {
		throw std::runtime_error ("Swap chain creation failed.");
	}

	return result;
}
}

///////////////////////////////////////////////////////////////////////////////
DX12Renderer::DX12Renderer() :
	frameFenceEvents_{ 0,0,0 },
	currentFenceValue_(0),
	fenceValues_{0,0,0},
	renderTargetViewDescriptorSize_(0),
	currentRenderState_(nullptr)
{
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
}

///////////////////////////////////////////////////////////////////////////////
DX12Renderer::~DX12Renderer ()
{
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::PrepareRender ()
{
	commandAllocators_ [currentBackBuffer_]->Reset ();

	auto commandList = commandLists_ [currentBackBuffer_].Get ();
	commandList->Reset (
		commandAllocators_ [currentBackBuffer_].Get (), nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandle;
	CD3DX12_CPU_DESCRIPTOR_HANDLE::InitOffsetted (renderTargetHandle, renderTargetDescriptorHeap_->GetCPUDescriptorHandleForHeapStart (), currentBackBuffer_, renderTargetViewDescriptorSize_);

	//Get a handle to the depth / stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(depthStencilDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

	commandList->OMSetRenderTargets (1, &renderTargetHandle, true, &dsvHandle);
	commandList->RSSetViewports (1, &viewport_);
	commandList->RSSetScissorRects (1, &rectScissor_);

	// Transition back buffer
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTargets_ [currentBackBuffer_].Get ();
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	commandList->ResourceBarrier (1, &barrier);

	static const float clearColor [] = {
		0.042f, 0.042f, 0.042f,
		1
	};

	commandList->ClearRenderTargetView (renderTargetHandle,clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

namespace {
	void WaitForFence(ID3D12Fence* fence, UINT64 completionValue, HANDLE waitEvent)
	{
		if (fence->GetCompletedValue() < completionValue) {
			fence->SetEventOnCompletion(completionValue, waitEvent);
			WaitForSingleObject(waitEvent, INFINITE);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::Render(const FrameGraph* frameGraph)
{
	WaitForFence(
		frameFences_[GetQueueSlot()].Get(),
		fenceValues_[GetQueueSlot()],
		frameFenceEvents_[GetQueueSlot()]);

	PrepareRender();

	auto commandList = commandLists_[currentBackBuffer_].Get();

	if (frameGraph->Meshes.size() > 0)
		DrawMeshes(frameGraph->RenderCam, frameGraph->Meshes, commandList);
	if (frameGraph->InstancedMeshes.size() > 0)
		DrawInstancedMeshes(frameGraph, commandList);
	if (frameGraph->UI.size() > 0)
		DrawMeshes(frameGraph->UICam, frameGraph->UI, commandList);
	
	FinalizeRender();

	Present();
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::DrawMesh(std::string id, ID3D12GraphicsCommandList* commandList, int instanceCount) 
{
	_STL_ASSERT((resMeshBuffers_.find(id) != resMeshBuffers_.end()), "Mesh with given Id couldn't be found");
	DX12MeshBuffer meshBuffer = resMeshBuffers_[id];
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &meshBuffer.vertexBufferView);
	commandList->IASetIndexBuffer(&meshBuffer.indexBufferView);
	commandList->DrawIndexedInstanced(meshBuffer.indexCount, instanceCount, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::DrawMeshes(const FrameGraphCamera& cam, const std::vector<FrameGraphMesh>& meshes, ID3D12GraphicsCommandList* commandList)
{
	SetRenderState(RENDERSTATE_DEFAULT, commandList);
	UpdateCBuffer(cam.CBuffer, commandList);

	for (const FrameGraphMesh& fgMesh : meshes)
	{
		// ------------------------------------------------------------- BIND TEXTURES
		for (const auto& fgTextureBind : fgMesh.TextureBinds)
		{
			SetRenderState(fgMesh.RenderState, commandList);

			_STL_ASSERT(resTextures_.find(fgTextureBind.Id) != resTextures_.end(), "Texture with given Id couldn't be found");
			DX12Texture texture = resTextures_[fgTextureBind.Id];
			UINT rootIndex = fgTextureBind.RootIndex;

			// Set the descriptor heap containing the texture srv
			ID3D12DescriptorHeap* heaps[] = { texture.srvDescriptorHeap.Get() };
			commandList->SetDescriptorHeaps(1, heaps);

			// Set slot 0 of our root signature to point to our descriptor heap with
			// the texture SRV
			commandList->SetGraphicsRootDescriptorTable(rootIndex,
				texture.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		}

		UpdateCBuffer(fgMesh.CBuffer, commandList);

		DrawMesh(fgMesh.Id, commandList, 1);
	}
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::DrawInstancedMeshes(const FrameGraph* frameGraph, ID3D12GraphicsCommandList* commandList) {

	SetRenderState(RENDERSTATE_DEFAULT_INSTANCED, commandList);

	const FrameGraphCamera fgCam = frameGraph->RenderCam;
	UpdateCBuffer(fgCam.CBuffer, commandList);

	for (const FrameGraphMesh& fgMesh : frameGraph->InstancedMeshes)
	{
		SetRenderState(fgMesh.RenderState, commandList);

		// ------------------------------------------------------------ - BIND TEXTURES
		for (const auto& fgTextureBind : fgMesh.TextureBinds)
		{
			_STL_ASSERT(resTextures_.find(fgTextureBind.Id) != resTextures_.end(), "Texture with given Id couldn't be found");
			DX12Texture texture = resTextures_[fgTextureBind.Id];
			UINT rootIndex = fgTextureBind.RootIndex;

			// Set the descriptor heap containing the texture srv
			ID3D12DescriptorHeap* heaps[] = { texture.srvDescriptorHeap.Get() };
			commandList->SetDescriptorHeaps(1, heaps);

			// Set slot 0 of our root signature to point to our descriptor heap with
			// the texture SRV
			commandList->SetGraphicsRootDescriptorTable(rootIndex,
				texture.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		}

		// Update Instance buffer
		_STL_ASSERT(resInstanceBuffers_.find(fgMesh.CBuffer.Id) != resInstanceBuffers_.end(), "Instance Buffer with given Id couldn't be found");
		DX12InstanceBuffer iBuffer = resInstanceBuffers_[fgMesh.CBuffer.Id];
		iBuffer.buffer.Update(GetQueueSlot(), fgMesh.CBuffer.Data);
		commandList->SetGraphicsRootShaderResourceView(2, iBuffer.buffer.slotData[GetQueueSlot()]->GetGPUVirtualAddress());

		DrawMesh(fgMesh.Id, commandList, iBuffer.instanceCount);
	}
}

void DX12Renderer::SetRenderState(std::string renderStateId, ID3D12GraphicsCommandList* commandList) {
	_STL_ASSERT(resRenderStates_.find(renderStateId) != resRenderStates_.end(), "RenderState couldn't be found. Make sure the material as a valid shader set");
	DX12RenderState* renderState = resRenderStates_[renderStateId];
	if (currentRenderState_ != renderState) {
		currentRenderState_ = renderState;
		commandList->SetGraphicsRootSignature(currentRenderState_->rootSignature.Get());
		commandList->SetPipelineState(currentRenderState_->pipelineState.Get());
	}
}


///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::UpdateCBuffer(const FrameGraphCBuffer& cbuffer, ID3D12GraphicsCommandList* commandList)
{
	_STL_ASSERT(resConstantBuffers_.find(cbuffer.Id) != resConstantBuffers_.end(), "CBuffer with given Id couldn't be found");
	DX12Buffer cBuffer = resConstantBuffers_[cbuffer.Id];
	cBuffer.Update(GetQueueSlot(), cbuffer.Data);
	// Set slot "rootIndex" of our root signature to the constant buffer view
	commandList->SetGraphicsRootConstantBufferView(cbuffer.RootIndex, cBuffer.slotData[GetQueueSlot()]->GetGPUVirtualAddress());
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::FinalizeRender ()
{
	// Transition the swap chain back to present
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Transition.pResource = renderTargets_ [currentBackBuffer_].Get ();
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	auto commandList = commandLists_ [currentBackBuffer_].Get ();
	commandList->ResourceBarrier (1, &barrier);

	commandList->Close ();

	// Execute our commands
	ID3D12CommandList* commandLists [] = { commandList };
	commandQueue_->ExecuteCommandLists (std::extent<decltype(commandLists)>::value, commandLists);

	currentRenderState_ = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
/**
Setup all render targets. This creates the render target descriptor heap and
render target views for all render targets.

This function does not use a default view but instead changes the format to
_SRGB.
*/
void DX12Renderer::SetupRenderTargets ()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtHeapDesc = {};
	rtHeapDesc.NumDescriptors = GetQueueSlotCount ();
	rtHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device_->CreateDescriptorHeap (&rtHeapDesc, IID_PPV_ARGS (&renderTargetDescriptorHeap_));


	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{ 
		renderTargetDescriptorHeap_->GetCPUDescriptorHandleForHeapStart () };

	for (int i = 0; i < GetQueueSlotCount (); ++i) {

		wchar_t name[25] = {};
		swprintf_s(name, L"Render target %u", i);
		renderTargets_[i]->SetName(name);

		D3D12_RENDER_TARGET_VIEW_DESC viewDesc;
		viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		viewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		viewDesc.Texture2D.MipSlice = 0;
		viewDesc.Texture2D.PlaneSlice = 0;

		device_->CreateRenderTargetView (renderTargets_ [i].Get (), &viewDesc,
			rtvHandle);

		rtvHandle.Offset (renderTargetViewDescriptorSize_);
	}


	// ---------------------------------------- Depth Buffer

	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
	dsvDescriptorHeapDesc.NumDescriptors = 1;
	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device_->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(depthStencilDescriptorHeap_.ReleaseAndGetAddressOf()));
	
	// Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
	// on this surface.
	CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

	D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		depthBufferFormat,
		window_->GetWidth(),
		window_->GetHeight(),
		1, // This depth stencil view has only one texture.
		1  // Use a single mipmap level.
	);
	depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = depthBufferFormat;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	device_->CreateCommittedResource(
		&depthHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(renderTargetDepthStencil_.ReleaseAndGetAddressOf())
	);


	renderTargetDepthStencil_->SetName(L"Depth stencil");
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = depthBufferFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	device_->CreateDepthStencilView(renderTargetDepthStencil_.Get(), &dsvDesc, depthStencilDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

}

///////////////////////////////////////////////////////////////////////////////
/**
Present the current frame by swapping the back buffer, then move to the
next back buffer and also signal the fence for the current queue slot entry.
*/
void DX12Renderer::Present ()
{
	swapChain_->Present (1, 0);

	// Mark the fence for the current frame.
	const auto fenceValue = currentFenceValue_;
	commandQueue_->Signal (frameFences_ [currentBackBuffer_].Get (), fenceValue);
	fenceValues_[currentBackBuffer_] = fenceValue;
	++currentFenceValue_;

	// Take the next back buffer from our chain
	currentBackBuffer_ = (currentBackBuffer_ + 1) % GetQueueSlotCount ();
}

///////////////////////////////////////////////////////////////////////////////
/**
Set up swap chain related resources, that is, the render target view, the
fences, and the descriptor heap for the render target.
*/
void DX12Renderer::SetupSwapChain ()
{
	currentFenceValue_ = 1;

	// Create fences for each frame so we can protect resources and wait for
	// any given frame
	for (int i = 0; i < GetQueueSlotCount (); ++i) {
		frameFenceEvents_ [i] = CreateEvent (nullptr, FALSE, FALSE, nullptr);
		fenceValues_ [i] = 0;
		device_->CreateFence (0, D3D12_FENCE_FLAG_NONE, 
			IID_PPV_ARGS (&frameFences_ [i]));
	}

	for (int i = 0; i < GetQueueSlotCount (); ++i) {
		swapChain_->GetBuffer (i, IID_PPV_ARGS (&renderTargets_ [i]));
	}

	SetupRenderTargets ();
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::Initialize (const Window &window)
{
	window_ = &window;

	CreateDeviceAndSwapChain (window);
	CreateAllocatorsAndCommandLists ();
	CreateViewportScissor (window);
	CreateDefaultRenderStates();
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::InitializeImpl (ID3D12GraphicsCommandList * /*uploadCommandList*/)
{
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::Shutdown ()
{
	// Drain the queue, wait for everything to finish
	for (int i = 0; i < GetQueueSlotCount(); ++i) {
		WaitForFence(frameFences_[i].Get(), fenceValues_[i], frameFenceEvents_[i]);
	}

	for (auto event : frameFenceEvents_) {
		CloseHandle (event);
	}
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::CreateDeviceAndSwapChain (const Window& window)
{
	// Enable the debug layers when in debug mode
	// If this fails, install the Graphics Tools for Windows. On Windows 10,
	// open settings, Apps, Apps & Features, Optional features, Add Feature,
	// and add the graphics tools
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debugController;
	D3D12GetDebugInterface (IID_PPV_ARGS (&debugController));
	debugController->EnableDebugLayer ();
#endif

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	::ZeroMemory (&swapChainDesc, sizeof (swapChainDesc));

	swapChainDesc.BufferCount = GetQueueSlotCount ();
	// This is _UNORM but we'll use a _SRGB view on this. See 
	// SetupRenderTargets() for details, it must match what
	// we specify here
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Width = window.GetWidth ();
	swapChainDesc.BufferDesc.Height = window.GetHeight ();
	swapChainDesc.OutputWindow = window.GetHWND ();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Windowed = true;

	auto renderEnv = CreateDeviceAndSwapChainHelper (nullptr, D3D_FEATURE_LEVEL_11_0,
		&swapChainDesc);

	device_ = renderEnv.device;
	commandQueue_ = renderEnv.queue;
	swapChain_ = renderEnv.swapChain;

	renderTargetViewDescriptorSize_ =
		device_->GetDescriptorHandleIncrementSize (D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	SetupSwapChain ();
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::CreateAllocatorsAndCommandLists ()
{
	for (int i = 0; i < GetQueueSlotCount (); ++i) {
		device_->CreateCommandAllocator (D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS (&commandAllocators_ [i]));
		device_->CreateCommandList (0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocators_ [i].Get (), nullptr,
			IID_PPV_ARGS (&commandLists_ [i]));
		commandLists_ [i]->Close ();
	}
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::CreateViewportScissor (const Window& window)
{
	rectScissor_ = { 0, 0, window.GetWidth (), window.GetHeight () };

	viewport_ = { 0.0f, 0.0f,
		static_cast<float>(window.GetWidth ()),
		static_cast<float>(window.GetHeight ()),
		0.0f, 1.0f
	};
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::CreateDefaultRenderStates() {
	resRenderStates_[RENDERSTATE_DEFAULT] = DX12RenderState::Create(device_.Get(), RENDERSTATE_DEFAULT, false);
	resRenderStates_[RENDERSTATE_DEFAULT_INSTANCED] = DX12RenderState::Create(device_.Get(), RENDERSTATE_DEFAULT_INSTANCED, true);
}

///////////////////////////////////////////////////////////////////////////////
void DX12Renderer::LoadResources(std::vector<GraphicResourceDesc> graphRes) {
	// Create our upload fence, command list and command allocator
	// This will be only used while creating the mesh buffer and the texture
	// to upload data to the GPU.
	ComPtr<ID3D12Fence> uploadFence;
	device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&uploadFence));

	ComPtr<ID3D12CommandAllocator> uploadCommandAllocator;
	device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&uploadCommandAllocator));
	ComPtr<ID3D12GraphicsCommandList> uploadCommandList;
	device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		uploadCommandAllocator.Get(), nullptr,
		IID_PPV_ARGS(&uploadCommandList));

	
	for (const GraphicResourceDesc &res : graphRes) 
	{
		if (res.Type == GraphicResourceDesc::ResourceType::Mesh) {
			Mesh* mesh = static_cast<Mesh*>(res.Data);
			_STL_ASSERT(mesh != nullptr, "Error casting the resource to a mesh type");
			DX12MeshBuffer meshBuffer = DX12MeshBuffer::Create(device_.Get(), *mesh, uploadCommandList.Get());
			resMeshBuffers_[res.Id] = meshBuffer;
		}
		else if (res.Type == GraphicResourceDesc::ResourceType::Texture) {
			Texture* texture = static_cast<Texture*>(res.Data);
			_STL_ASSERT(texture != nullptr, "Error casting the resource to a texture type");
			DX12Texture dx12texture = DX12Texture::Create(device_.Get(), *texture, uploadCommandList.Get(), res.Id);
			resTextures_[res.Id] = dx12texture;
		}
		else if (res.Type == GraphicResourceDesc::ResourceType::ConstantBuffer) {
			ConstantBufferDef* cbDef = static_cast<ConstantBufferDef*>(res.Data);
			_STL_ASSERT(cbDef != nullptr, "Error casting the resource to a constant buffer type");
			std::string name = "ConstantBuffer ";
			name.append(res.Id);
			DX12Buffer cBuffer = DX12Buffer::Create(device_.Get(), GetQueueSlotCount(), cbDef->size, cbDef->ptr, name);
			resConstantBuffers_[res.Id] = cBuffer;
		}
		else if (res.Type == GraphicResourceDesc::ResourceType::InstanceBuffer) {
			InstanceBufferDef* ibDef = static_cast<InstanceBufferDef*>(res.Data);
			_STL_ASSERT(ibDef != nullptr, "Error casting the resource to a instance buffer type");
			DX12InstanceBuffer iBuffer;
			std::string name = "InstanceBuffer ";
			name.append(res.Id);
			iBuffer.buffer = DX12Buffer::Create(device_.Get(), GetQueueSlotCount(), ibDef->size, ibDef->ptr, name);
			iBuffer.instanceCount = ibDef->instanceCount;
			resInstanceBuffers_[res.Id] = iBuffer;
		}
		else if (res.Type == GraphicResourceDesc::ResourceType::RenderState) {
			RenderStateDef* sDef = static_cast<RenderStateDef*>(res.Data);
			_STL_ASSERT(sDef != nullptr, "Error casting the resource to a shader def type");
			ID3D12RootSignature* pipeline = nullptr;
			pipeline = sDef->instancing ? rootSignatureInstancing_.Get() : rootSignature_.Get();
			if (resRenderStates_.find(sDef->shaderPath) == resRenderStates_.end()) // RenderState still doesn't exist
				resRenderStates_[res.Id] = DX12RenderState::Create(device_.Get(), sDef->shaderPath, sDef->instancing);
		}
	}

	uploadCommandList->Close();

	// Execute the upload and finish the command list
	ID3D12CommandList* commandLists[] = { uploadCommandList.Get() };
	commandQueue_->ExecuteCommandLists(std::extent<decltype(commandLists)>::value, commandLists);
	commandQueue_->Signal(uploadFence.Get(), 1);

	auto waitEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (waitEvent == NULL) {
		throw std::runtime_error("Could not create wait event.");
	}

	WaitForFence(uploadFence.Get(), 1, waitEvent);

	// Cleanup our upload handle
	uploadCommandAllocator->Reset();

	CloseHandle(waitEvent);
}
