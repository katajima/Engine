#include"Object3d.hlsli"
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
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b5);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
    
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    float4 transformedUV = mul(float4(input.texcoord.xy, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(sSampler, transformedUV.xy);
    
    
    float3 normal = input.normal;
    float3 tangent = input.tangent;
    float3 biNormal = input.biNormal;
    if (gMaterial.useNormalMap)
    {
        normal = normalize(mul((float3x3) gTransformationMatrix.WorldInverseTranspose, input.normal));
        tangent = normalize(mul((float3x3) gTransformationMatrix.World, input.tangent));
        biNormal = normalize(cross(normal, tangent)); // バイノーマルを再計算
    }
    
    if (gMaterial.enableLighting != 0) // Lightingする場合
    {      
        // サンプリング
        float amdientPower = 0;
        if (gMaterial.useNormalMap)
        {
            float3 localNormal = g_Normalmap.Sample(sSampler, input.texcoord).xyz * 2.0f - 1.0f;

    // TBN行列（タンジェント・バイノーマル・ノーマル）でワールド空間の法線を計算
            float3x3 TBN = float3x3(input.tangent, input.biNormal, input.normal);
            normal = normalize(mul(localNormal, TBN));
        }
        
            
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        
        
        // 平行光原
        float3 directionalLig = { 0, 0, 0 }; // 環境光 
        // ポイントライトの処理
        float3 pointLig = { 0, 0, 0 };
 
        float3 allDire = DirectionalLightFunc(input, textureColor, toEye, normal);
        float3 allPoint = PointLightFunc(input, textureColor, toEye, normal);
        float3 allSpot = SpotLightFunc(input, textureColor, toEye, normal);
        
        // リグを使うか
        if (gMaterial.useLig != 0)
        {
          
            if (gDirectionalLight.enableLighting)
            {
                allDire *= directionalLig;
               
            }
        }
        
        
        
        output.color.rgb = allDire + allPoint + allSpot;
        
        
        output.color.a = gMaterial.color.a * textureColor.a;

        if (textureColor.a <= 0.5f)
        {
            discard;
        }
        if (output.color.a <= 0.0f)
        {
            discard;
        }
        
    }
    else // Lightingしない場合。前回までと同じ演算
    {
        output.color = gMaterial.color * textureColor;
    }
    
    
    
    output.color = pow(output.color, 2.2f);
    
    return output;
}