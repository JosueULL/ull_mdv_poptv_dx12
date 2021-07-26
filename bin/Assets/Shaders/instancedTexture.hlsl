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
};

struct InstanceData
{
	float4x4 transform;	
};

StructuredBuffer<InstanceData> gInstanceData : register(t1);

VertexShaderOutput VS_main(float4 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD,  uint instanceID : SV_InstanceID)
{
	VertexShaderOutput output;
	InstanceData idata = gInstanceData[instanceID];
	float4x4 mvp = mul(mProj, mul(mView, idata.transform));	
	output.position = mul(mvp, float4(position.xyz, 1));
	output.wNormal = mul(idata.transform, normal);
	output.uv = uv;

	return output;
}

Texture2D<float4> anteruTexture : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float nDotL = clamp(dot(normalize(IN.wNormal), normalize(float3(0.25,1,0))), 0.25, 1);
	float3 col = anteruTexture.Sample(texureSampler, IN.uv) * nDotL;
	return float4(col,1);
}