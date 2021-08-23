struct InstanceData
{
	float4x4 transform;	
};

StructuredBuffer<InstanceData> gInstanceData : register(t1);

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

