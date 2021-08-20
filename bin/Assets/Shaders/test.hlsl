cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}


cbuffer SharedConstants : register (b2)
{
	float4 time; // elapsedTime, sin(elapsedTime)
}

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 wNormal : NORMAL;
	float2 uv : TEXCOORD;
	float3 localPos : TEXCOORD1;
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
	output.localPos = position.xyz;
	output.position = mul(mvp, float4(position.xyz, 1));
	output.position.y += sin(time.x*5) * 0.25;

	output.wNormal = mul(idata.transform, normal);
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
	//depth *= depth*depth;
	float3 col = nDotL * depth * tCol1.r * tCol2.g * lerp( float3(1,0, 0.25), float3(0.15,0.25,0.25), intensity*0.5);
	//if (length(col)< 0.05)
//		discard;
	return float4(col,1);
}