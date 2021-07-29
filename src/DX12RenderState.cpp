#include "DX12RenderState.h"

#include "d3dx12.h"
#include <fstream>
#include <string>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;

///////////////////////////////////////////////////////////////////////////////
DX12RenderState* DX12RenderState::Create(ID3D12Device* device, std::string shaderPath, bool instancing)
{
	DX12RenderState* renderState = new DX12RenderState();
	renderState->CreateRootSignature(device, instancing);
	renderState->CreatePipelineState(device, shaderPath);
	return renderState;
}


void DX12RenderState::CreateRootSignature(ID3D12Device* device, bool instancing) {
	CD3DX12_ROOT_PARAMETER parameters[3];

	// Create a descriptor table with one entry in our descriptor heap
	CD3DX12_DESCRIPTOR_RANGE range{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0 };
	parameters[0].InitAsDescriptorTable(1, &range);										// Add srv descriptor table (for textures)
	parameters[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);		// Add constant buffer view (camera data) as a descriptor
	if (instancing)
		parameters[2].InitAsShaderResourceView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);		// Add SRV for StructuredBuffer
	else
		parameters[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);		// Add constant buffer view (object data) as a descriptor 

	// We don't use another descriptor heap for the sampler, instead we use a static sampler
	CD3DX12_STATIC_SAMPLER_DESC samplers[1];
	samplers[0].Init(0, D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT);

	CD3DX12_ROOT_SIGNATURE_DESC descRootSignature(3, parameters, 1, samplers, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootBlob, errorBlob;
	D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &rootBlob, &errorBlob);

	device->CreateRootSignature(0, rootBlob->GetBufferPointer(), rootBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
}

void DX12RenderState::CreatePipelineState(ID3D12Device* device, std::string shaderPath)
{
	static const D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	std::ifstream ifs(shaderPath);
	_STL_ASSERT(ifs.good(), "Shader file doesn't exist");
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
	psoDesc.pRootSignature = rootSignature.Get();
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

	device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
}