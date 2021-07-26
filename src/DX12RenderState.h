#pragma once

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include <string>

struct DX12RenderState {
public:
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	
	static DX12RenderState Create(ID3D12Device* device, ID3D12RootSignature* rootSign, std::string shaderPath);

};