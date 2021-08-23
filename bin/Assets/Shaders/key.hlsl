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
	float2 uv : TEXCOORD0;
};

struct InstanceData
{
	float4x4 transform;	
};

StructuredBuffer<InstanceData> gInstanceData : register(t1);
Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);

// Rotation with angle (in radians) and axis
float3x3 AngleAxis3x3(float angle, float3 axis)
{
    float c, s;
    sincos(angle, s, c);

    float t = 1 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return float3x3(
        t * x * x + c,      t * x * y - s * z,  t * x * z + s * y,
        t * x * y + s * z,  t * y * y + c,      t * y * z - s * x,
        t * x * z - s * y,  t * y * z + s * x,  t * z * z + c
    );
}

VertexShaderOutput VS_main(VertexShaderInput IN)
{
	VertexShaderOutput output;
	InstanceData idata = gInstanceData[IN.instanceID];
	float4x4 mvp = mul(mProj, mul(mView, idata.transform));
	float3x3 rot = AngleAxis3x3(time.x, float3(0,1,0));
	
	output.position = mul(mvp, float4(mul(rot, IN.position.xyz), 1));
	output.wNormal = mul(idata.transform, float4(mul(rot, IN.normal),0)).xyz;
	output.uv = IN.uv;

	return output;
}

float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{
	float3 lightDir = normalize(float3(0.25,1,0));
	float3 viewDir = mView[2].xyz;
	float nDotL = clamp(dot(normalize(IN.wNormal), lightDir), 0.1, 1);
	
	
	float3 H = normalize(lightDir + viewDir);
	float NdotH = dot(IN.wNormal, H);
	float specular = pow(saturate(NdotH), 1);
	

	float fresnel = abs(dot(viewDir, IN.wNormal)) * 0.5;
	float4 col = mainTex.Sample(texureSampler, IN.uv);
	return saturate(col * nDotL * fresnel) + col * pow(1-fresnel*2,2) + specular;
}