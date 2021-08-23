#include "common.hlsli"
#include "singleCommon.hlsli"

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	return mainTex.Sample(texureSampler, IN.uv);
}