#include "DX12RenderState.h"

#include "d3dx12.h"
#include <fstream>
#include <string>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;

///////////////////////////////////////////////////////////////////////////////
DX12RenderState DX12RenderState::Create(ID3D12Device* device, ID3D12RootSignature* rootSign, std::string shaderPath)
{
	DX12RenderState renderState;

	static const D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	std::ifstream ifs(shaderPath);
	std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	const char* sc = content.c_str();

	ComPtr<ID3DBlob> vertexShader, pixelShader, errorBlob;
	D3DCompile(sc, strlen(sc),
		"", nullptr, nullptr,
		"VS_main", "vs_5_1", 0, 0, &vertexShader, &errorBlob);
	if (errorBlob)
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	D3DCompile(sc, strlen(sc),
		"", nullptr, nullptr,
		"PS_main", "ps_5_1", 0, 0, &pixelShader, &errorBlob);
	if (errorBlob)
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.VS.BytecodeLength = vertexShader->GetBufferSize();
	psoDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
	psoDesc.PS.BytecodeLength = pixelShader->GetBufferSize();
	psoDesc.PS.pShaderBytecode = pixelShader->GetBufferPointer();
	psoDesc.pRootSignature = rootSign;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.InputLayout.NumElements = std::extent<decltype(layout)>::value;
	psoDesc.InputLayout.pInputElementDescs = layout;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// Simple alpha blending
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = 0xFFFFFFFF;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&renderState.pipelineState));

	return renderState;
}
