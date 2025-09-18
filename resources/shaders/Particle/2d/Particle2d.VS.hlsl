#include "Particle2d.hlsli"

StructuredBuffer<ParticleGPU> gParticle : register(t1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    
    output.color = gParticle[instanceId].color;
    return output;
}