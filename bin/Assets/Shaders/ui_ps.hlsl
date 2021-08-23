#include "common.hlsli"
#include "singleCommon.hlsli"

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float4 col = mainTex.Sample(texureSampler, IN.uv);
	if (col.a < 0.1)
		discard;
	return col;
}