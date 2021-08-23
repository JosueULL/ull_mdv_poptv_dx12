cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}

cbuffer SharedConstants : register (b2)
{
	float4 time; // elapsedTime, sin(elapsedTime)
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

VertexShaderOutput VS_main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD)
{
	VertexShaderOutput output;

	float4x4 mvp = mul(mProj, mul(mView, mWorld));	
	output.position = mul(mvp, float4(position.xyz, 1));
	output.wNormal = mul(mWorld, float4(normal, 0)).xyz;
	output.uv = uv;

	return output;
}

Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float4 tCol1 = mainTex.Sample(texureSampler, IN.uv*0.25 + float2(0,time.x*0.1));
	float4 tCol2 = mainTex.Sample(texureSampler, IN.uv + tCol1.rg + float2(0,-time.x*0.25));
	float center = abs((IN.uv.y-0.5)*2);
	float border = 1-center;
	if (border * tCol2.g < 0.025)
		discard;

	return lerp(float4(0,0,0,1), float4(0.025,0.025,0.9,1), pow(tCol2.r+border,2))*pow(IN.uv.y,2);
}