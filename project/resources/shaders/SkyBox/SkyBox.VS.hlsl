#include "SkyBox.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float3 texcoord : TEXCOORD0;
};

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);


VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position,gTransformationMatrix.WVP).xyww;
    output.texcoord = input.position.xyz;
    
    
    return output;
}