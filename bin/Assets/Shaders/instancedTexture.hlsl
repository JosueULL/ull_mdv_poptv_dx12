cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}

cbuffer SharedConstants : register (b2)
{
	float4 time; // elapsedTime, sin(elapsedTime)
}

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 wNormal : NORMAL;
	float2 uv : TEXCOORD1;
};

struct InstanceData
{
	float4x4 transform;	
};

StructuredBuffer<InstanceData> gInstanceData : register(t1);
Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	VertexShaderOutput output;
	InstanceData idata = gInstanceData[IN.instanceID];
	float4x4 mvp = mul(mProj, mul(mView, idata.transform));	
	output.position = mul(mvp, float4(IN.position.xyz, 1));
	output.wNormal = mul(idata.transform, IN.normal);
	output.uv = IN.uv;

	return output;
}

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	return mainTex.Sample(texureSampler, IN.uv);
}