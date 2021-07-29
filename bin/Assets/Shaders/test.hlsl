cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}
cbuffer ObjectConstants : register (b1)
{
	float4x4 mWorld;
}

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 wNormal : NORMAL;
	float2 uv : TEXCOORD;
	float3 color : COLOR;
};

VertexShaderOutput VS_main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD,
	float3 color : COLOR)
{
	VertexShaderOutput output;

	float4x4 mvp = mul(mProj, mul(mView, mWorld));	
	output.position = mul(mvp, float4(position.xyz, 1));
	output.wNormal = mul(mWorld, normal);
	output.uv = uv;
	output.color = color;

	return output;
}

Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float nDotL = clamp(dot(normalize(IN.wNormal), normalize(float3(0.25,1,0))), 0.25, 1);
	float3 col = lerp(float4(0,1,0,1), float4(1,0,1,0),  sin(IN.position.y*0.5));
	return float4(IN.color,1);
}