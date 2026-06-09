#include"Object3dInstans.hlsli"


StructuredBuffer<Object3dGPU> gParticle : register(t0);


struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    float4 tangent : TANGENT0; // 接ベクトル
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
    
    
     // 頂点シェーダ側
    float3 transformedNormal = normalize(mul((float3x3) gParticle[instanceId].worldInverseTranspose, input.normal));
    float3 transformedTangent = normalize(mul((float3x3) gParticle[instanceId].World, input.tangent.xyz));
    // 従ベクトルは頂点に持たず、接ベクトルのwに入っている向きで復元する
    float3 transformedBinormal = normalize(cross(transformedNormal, transformedTangent) * input.tangent.w); // ←重要！

    output.normal = transformedNormal;
    output.tangent = transformedTangent;
    output.biNormal = transformedBinormal;
    
    return output;
}

