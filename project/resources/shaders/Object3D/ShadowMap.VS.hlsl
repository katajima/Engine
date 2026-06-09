#include"object3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct ShadowData
{
    float4x4 lightViewProjection;
    float3 lightDirection;
    float bias;
};
ConstantBuffer<ShadowData> gShadowData : register(b6);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
};

float4 main(VertexShaderInput input) : SV_POSITION
{
    // ワールド座標をライト視点へ変換し、深度だけをシャドウマップに書き込む。
    float4 worldPosition = mul(input.position, gTransformationMatrix.World);
    return mul(worldPosition, gShadowData.lightViewProjection);
}
