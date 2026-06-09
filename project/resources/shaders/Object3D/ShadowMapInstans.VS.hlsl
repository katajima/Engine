#include"Object3dInstans.hlsli"

StructuredBuffer<Object3dGPU> gParticle : register(t0);

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

float4 main(VertexShaderInput input, uint instanceId : SV_InstanceID) : SV_POSITION
{
    // 各インスタンスのWorld行列を使い、ライト視点の深度だけを書き込む。
    float4 worldPosition = mul(input.position, gParticle[instanceId].World);
    return mul(worldPosition, gShadowData.lightViewProjection);
}
