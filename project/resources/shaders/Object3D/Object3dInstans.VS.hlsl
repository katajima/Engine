#include"Object3dInstans.hlsli"


StructuredBuffer<Object3dGPU> gParticle : register(t0);

//struct TransformationMatrix
//{
//    float4x4 WVP;
//    float4x4 World;
//    float4x4 WorldInverseTranspose;
//};
//ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceId].color;
    output.textureIndex = gParticle[instanceId].textureIndex;
    output.worldPosition = mul(input.position, gParticle[instanceId].World).xyz;
    return output;
}

