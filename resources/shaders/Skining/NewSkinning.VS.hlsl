#include "SkinningObject3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

// Compute Shader でスキニング済みの頂点データを前提とした入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
    float3 biNormal : BINORMAL0;
    
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.biNormal = normalize(mul((float3x3) gTransformationMatrix.World, input.biNormal));
    output.tangent = normalize(mul((float3x3) gTransformationMatrix.World, input.tangent.xyz));
    output.transformedNormal = normalize(mul((float3x3) gTransformationMatrix.WorldInverseTranspose, input.normal));

    return output;
}
