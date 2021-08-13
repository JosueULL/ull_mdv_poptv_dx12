cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}
cbuffer ObjectConstants : register (b1)
{
	float4x4 mWorld;
}
cbuffer SharedConstants : register (b2)
{
	float4 time; // elapsetTime, sin(elapsedTime)
}

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 wNormal : NORMAL;
	float2 uv : TEXCOORD;
};

VertexShaderOutput VS_main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD)
{
	VertexShaderOutput output;

	float4x4 mvp = mul(mProj, mul(mView, mWorld));	
	output.position = mul(mvp, float4(position.xyz, 1));
	output.wNormal = mul(mWorld, normal);
	output.uv = uv;

	return output;
}

Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float4 col = mainTex.Sample(texureSampler, IN.uv);
	if (col.a < 0.1)
		discard;
	return col;
}