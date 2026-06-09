#include "SkinningObject3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

// Compute Shader �ŃX�L�j���O�ς݂̒��_�f�[�^��O��Ƃ�������
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
    
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.WorldInverseTranspose));
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;
    output.tangent = normalize(mul((float3x3) gTransformationMatrix.World, input.tangent.xyz));
    // 従ベクトルは頂点に持たず、接ベクトルのwに入っている向きで復元する
    output.biNormal = normalize(cross(output.normal, output.tangent) * input.tangent.w);
    output.transformedNormal = normalize(mul((float3x3) gTransformationMatrix.WorldInverseTranspose, input.normal));

    return output;
}
