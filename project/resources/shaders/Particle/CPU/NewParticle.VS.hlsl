#include"NewParticle.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};

struct PreView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};



StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PreView> gPreView : register(b0);



VertexShaderOutput main(VertexShaderInput input, uint instancId : SC_InstanceID)
{
    VertexShaderOutput output;
    
    
    return output;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}