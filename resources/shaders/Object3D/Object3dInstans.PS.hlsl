#include"Object3dInstans.hlsli"


Texture2D<float4> gTextures[] : register(t1);
SamplerState sSampler : register(s0);


struct Camera
{
    float3 worldPosition;
    float3 normal;
};
ConstantBuffer<Camera> gCamera : register(b2);

// 平行光線
struct DirectionalLight
{
    float4 color; //!< ライトの色
    float3 direction; //!< ライトの向き
    float intensity; //!< 輝度
    float ilg; // リグ
    int enableLighting;
    float3 groundColor; // 地面色
    float3 skyColor; // 天球色
    float3 groundNormal; // 地面法線方向
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Material
{
    
    float4 color;
    int enableLighting;
    float alphaClipping;
    float environmentCoefficient;
    float alpha;
    float4x4 uvTransform;
    float shininess;
    int useLig;
    int useHem;
    int useNormalMap;
    int useSpeculerMap;
};
ConstantBuffer<Material> gMaterial : register(b0);


float3 DirectionalLightFunc2(VertexShaderOutput input, float4 textureColor, float3 viewDir, float3 normal)
{
    float3 result = float3(0, 0, 0);
    if (gDirectionalLight.enableLighting)
    {
        float3 L = -normalize(gDirectionalLight.direction);
        normal = normalize(normal); // ここで一度だけ正規化
        viewDir = normalize(viewDir);

        // After（なめらかにする）
        float halfLambert = saturate(dot(normal, L) * 0.5f + 0.5f);
        halfLambert = smoothstep(0.0f, 1.0f, halfLambert);
        float3 diffuse = gMaterial.color.rgb * input.color.rgb * textureColor.rgb * halfLambert * gDirectionalLight.intensity;

        float spec = 0.0f;
        if (gMaterial.shininess >= 1.0f)
        {
            float3 H = normalize(viewDir + L);
            spec = pow(saturate(dot(normal, H)), gMaterial.shininess);
        }

        float3 specular = gDirectionalLight.color.rgb * input.color.rgb * spec * gDirectionalLight.intensity;
        result = diffuse + specular;
    }
    return result;
}



////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
  
    
    float4 transformedUV = mul(float4(input.texcoord.xy, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTextures[int(input.textureIndex)].Sample(sSampler, transformedUV.xy);
    //float4 textureColor = gTextures[int(input.textureIndex)].Sample(sSampler, input.texcoord);
   
    if (gMaterial.enableLighting != 0)
    {
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 normal = input.normal;
    
        float3 allDire = DirectionalLightFunc2(input, textureColor, toEye, normal);
   
   
        output.color.rgb = allDire;
        
        
        output.color.a = gMaterial.color.a * textureColor.a * gMaterial.alpha * input.color.a;

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
    else
    {
        output.color = gMaterial.color * textureColor * gMaterial.alpha * input.color;
        
        
        if (textureColor.a <= gMaterial.alphaClipping)
        {
            discard;
        }
        if (output.color.a <= 0.0f)
        {
            discard;
        }
    }
    
    
    return output;
}