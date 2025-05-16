#include"Light.hlsli"


Texture2D<float4> gTexture : register(t0);
Texture2D<float4> g_Normalmap : register(t1); // t1レジスタにバインドされる法線マップデータ
Texture2D<float4> g_Specularmap : register(t2); // t2レジスタにバインドされるスペキュラーマップデータ
Texture2D<float4> g_aoMap : register(t3); // t3レジスタにバインドされるスペキュラーマップデータ
SamplerState sSampler : register(s0);

struct Camera
{
    float3 worldPosition;
    float3 normal;
};
ConstantBuffer<Camera> gCamera : register(b2);

struct TransformationMatrix
{
    float4x4 WVP;
    float4x4 World;
    float4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b5);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord.xy, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(sSampler, transformedUV.xy);
    
    
    
    if (gMaterial.enableLighting != 0) // Lightingする場合
    {      
        
        float3 normal = input.normal;
        float3 tangent = input.tangent;
        float3 biNormal = input.biNormal;
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);

        if (gMaterial.useNormalMap)
        {
            float3 localNormal = g_Normalmap.Sample(sSampler, transformedUV.xy).xyz * 2.0f - 1.0f;
            
            float3x3 TBN = (float3x3(input.tangent.xyz, input.biNormal, input.normal)); // または必要なら transpose
            float3 worldNormal = normalize(mul(localNormal, TBN));

            normal = worldNormal;
        }

        float3 allDire = DirectionalLightFunc2(input, textureColor, toEye, normal);
        float3 allPoint = PointLightFunc(input, textureColor, toEye, normal);
        float3 allSpot = SpotLightFunc(input, textureColor, toEye, normal);

        
        
        
        
        
        output.color.rgb = allDire + allPoint + allSpot;
        
        
        output.color.a = gMaterial.color.a * textureColor.a;

        if (textureColor.a <= gMaterial.alphaClipping)
        {
            discard;
        }
        if (output.color.a <= 0.0f)
        {
            discard;
        }
        output.color = pow(output.color, 2.2f);
    }
    else // Lightingしない場合。前回までと同じ演算
    {
        output.color = gMaterial.color * textureColor;
    }
    
    
    
  
    
    return output;
}