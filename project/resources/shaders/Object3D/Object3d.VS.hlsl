#include"object3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

 
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    // 追加
    float4 tangent : TANGENT0; // 接ベクトル
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    output.texcoord = input.texcoord;
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;

    // 頂点シェーダ側
    float3 transformedNormal = normalize(mul((float3x3) gTransformationMatrix.WorldInverseTranspose, input.normal));
    float3 transformedTangent = normalize(mul((float3x3) gTransformationMatrix.World, input.tangent.xyz));
    // 従ベクトルは頂点に持たず、接ベクトルのwに入っている向きで復元する
    float3 transformedBinormal = normalize(cross(transformedNormal, transformedTangent) * input.tangent.w); // ←重要！

    output.normal = transformedNormal;
    output.tangent = transformedTangent;
    output.biNormal = transformedBinormal;

    return output;
}



