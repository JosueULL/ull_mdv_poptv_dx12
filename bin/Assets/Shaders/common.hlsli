cbuffer SharedConstants : register (b2)
{
	float4 time; // elapsedTime, sin(elapsedTime)
}

cbuffer CameraConstants : register (b0)
{
	float4x4 mView;
	float4x4 mProj;
}

Texture2D<float4> mainTex : register(t0);
SamplerState texureSampler      : register(s0);