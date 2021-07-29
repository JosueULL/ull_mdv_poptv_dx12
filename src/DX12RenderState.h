#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include <string>

struct DX12RenderState {
public:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	
	static DX12RenderState* Create(ID3D12Device* device, std::string shaderPath, bool instancing);

	void CreateRootSignature(ID3D12Device* device, bool instancing);
	void CreatePipelineState(ID3D12Device* device, std::string shaderPath);
};