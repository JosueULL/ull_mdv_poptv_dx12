cbuffer PerFrameConstants : register (b0)
{
	float4 scale;
}

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VertexShaderOutput VS_main(
	float4 position : POSITION,
	float2 uv : TEXCOORD)
{
	VertexShaderOutput output;

	output.position = position;
	//output.position.xy *= scale.x;
	output.uv = uv;

	return output;
}

Texture2D<float4> tex : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main (float4 position : SV_POSITION,
				float2 uv : TEXCOORD) : SV_TARGET
{
	return tex.Sample (texureSampler, uv);
}