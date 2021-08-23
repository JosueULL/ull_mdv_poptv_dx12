#include "common.hlsli"
#include "instancedCommon.hlsli"

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	VertexShaderOutput output;
	InstanceData idata = gInstanceData[IN.instanceID];
	float4x4 mvp = mul(mProj, mul(mView, idata.transform));	
	output.position = mul(mvp, float4(IN.position.xyz, 1));
	output.wNormal = mul(idata.transform, float4(IN.normal, 0)).xyz;
	output.uv = IN.uv;

	return output;
}