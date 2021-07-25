const char Shaders[] = 
"#ifdef D3D12_SAMPLE_BASIC\n"
"struct VertexShaderOutput\n"
"{\n"
"	float4 position : SV_POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n"
"\n"
"VertexShaderOutput VS_main(\n"
"	float4 position : POSITION,\n"
"	float2 uv : TEXCOORD)\n"
"{\n"
"	VertexShaderOutput output;\n"
"\n"
"	output.position = position;\n"
"	output.uv = uv;\n"
"\n"
"	return output;\n"
"}\n"
"\n"
"float4 PS_main (float4 position : SV_POSITION,\n"
"				float2 uv : TEXCOORD) : SV_TARGET\n"
"{\n"
"	return float4(uv, 0, 1);\n"
"}\n"
"#elif D3D12_SAMPLE_CONSTANT_BUFFER\n"
"cbuffer CameraConstants : register (b0)\n"
"{\n"
"	float4 scale;\n"
"}\n"
"cbuffer ObjectConstants : register (b1)\n"
"{\n"
"	float4 view;\n"
"}\n"
"\n"
"struct VertexShaderOutput\n"
"{\n"
"	float4 position : SV_POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n"
"\n"
"VertexShaderOutput VS_main(\n"
"	float4 position : POSITION,\n"
"	float2 uv : TEXCOORD)\n"
"{\n"
"	VertexShaderOutput output;\n"
"\n"
"	output.position = position;\n"
"	output.position.xy *= scale.x;\n"
"	output.uv = uv;\n"
"\n"
"	return output;\n"
"}\n"
"\n"
"float4 PS_main (float4 position : SV_POSITION,\n"
"				float2 uv : TEXCOORD) : SV_TARGET\n"
"{\n"
"	return float4(uv, 0, 1);\n"
"}\n"
"#elif D3D12_SAMPLE_TEXTURE\n"


// REGISTERS
"cbuffer CameraConstants : register (b0)\n"
"{\n"
"	float4x4 mView;\n"
"	float4x4 mProj;\n"
"}\n"
"cbuffer ObjectConstants : register (b1)\n"
"{\n"
"	float4x4 mWorld;\n"
"}\n"
"\n"
"struct VertexShaderOutput\n"
"{\n"
"	float4 position : SV_POSITION;\n"
"	float2 uv : TEXCOORD;\n"
"};\n"

// VERTEX SHADER
"\n"
"VertexShaderOutput VS_main(\n"
"	float4 position : POSITION,\n"
"	float2 uv : TEXCOORD)\n"
"{\n"
"	VertexShaderOutput output;\n"
"\n"

"	float4x4 mvp = mul(mProj, mul(mView, mWorld));"
"	output.position = mul(mvp, float4(position.xyz, 1));\n"
"	output.uv = uv;\n"
"\n"
"	return output;\n"
"}\n"

// PIXEL SHADER
"\n"
"Texture2D<float4> anteruTexture : register(t0);\n"
"SamplerState texureSampler      : register(s0);\n"
"\n"
"float4 PS_main (float4 position : SV_POSITION,\n"
"				float2 uv : TEXCOORD) : SV_TARGET\n"
"{\n"
"	return anteruTexture.Sample (texureSampler, uv);\n"
"}\n"



"#endif\n"
;
