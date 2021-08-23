#include "common.hlsli"
#include "singleCommon.hlsli"

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	VertexShaderOutput output;

	float4x4 mvp = mul(mProj, mul(mView, mWorld));	
	output.position = mul(mvp, float4(IN.position.xyz, 1));
	output.wNormal = mul(mWorld, float4(IN.normal, 0)).xyz;
	output.uv = IN.uv;

	return output;
}
