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
	float3 localPos : TEXCOORD1;
};

VertexShaderOutput VS_main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD,
	float3 color : COLOR)
{
	VertexShaderOutput output;

	float4x4 mvp = mul(mProj, mul(mView, mWorld));	
	output.localPos = position.xyz;
	output.position = mul(mvp, float4(position.xyz, 1));
	output.position.y += sin(time.x*2) * 0.1;

	output.wNormal = mul(mWorld, normal);
	output.uv = uv;

	return output;
}

Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float nDotL = clamp(dot(normalize(IN.wNormal), normalize(float3(0.25,1,0))), 0.25, 1);
	float depth = IN.position.z / IN.position.w;

	float4 tCol1 = mainTex.Sample(texureSampler, IN.uv*0.15 + float2(0,time.x*0.01));
	float4 tCol2 = mainTex.Sample(texureSampler, IN.uv*0.25 + float2(0,time.x*0.025));

	float intensity = (IN.localPos.y + tCol2.b);
	nDotL += tCol1.r*2;
	depth *= depth*depth;
	float3 col = nDotL * depth * tCol1.r * tCol2.g * lerp( float3(1,0, 0.25), float3(0.15,0.25,0.25), intensity*0.5);
	//if (length(col)< 0.05)
//		discard;
	return float4(col,1);
}