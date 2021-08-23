cbuffer ObjectConstants : register (b1)
{
	float4x4 mWorld;
}

struct VertexShaderInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 wNormal : NORMAL;
	float2 uv : TEXCOORD;
};
