#include "GpuParticle.hlsli"

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};




StructuredBuffer<Particle> gParticles : register(t1);
ConstantBuffer<PerView> gPerView : register(b0);


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    Particle particle = gParticles[instanceId];
    
    float4x4 worldMatrix = gPerView.billboardMatrix;
    
    worldMatrix[0] *= particle.scale.x;             // Scale X   
    worldMatrix[1] *= particle.scale.y;             // Scale Y
    worldMatrix[2] *= particle.scale.z;             // Scale Z
    worldMatrix[3].xyz = particle.translate;        // Translate
    worldMatrix[3].w = 1.0f;
    
    output.position = mul(input.position, mul(worldMatrix,gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;  
    return output;
}


