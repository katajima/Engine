#include"LightInstance.hlsli"


SamplerState sSampler           : register(s0);
SamplerComparisonState sShadowSampler : register(s1);
Texture2D<float4> gTextures[]   : register(t4);
Texture2D<float4> g_Normalmap   : register(t1); // t1レジスタにバインドされる法線マップデータ
Texture2D<float4> g_Specularmap : register(t2); // t2レジスタにバインドされるスペキュラーマップデータ
Texture2D<float4> g_aoMap       : register(t3); // t3レジスタにバインドされるスペキュラーマップデータ
Texture2D<float> gShadowMap     : register(t0);

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

struct ShadowData
{
    float4x4 lightViewProjection;
    float3 lightDirection;
    float bias;
    float shadowMin;
    float pcfRadius;
    float padding0;
    float padding1;
};
ConstantBuffer<ShadowData> gShadowData : register(b6);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;

};

float CalculateDepthShadow(float3 worldPosition, float3 normal)
{
    float4 shadowPosition = mul(float4(worldPosition, 1.0f), gShadowData.lightViewProjection);
    float3 projection = shadowPosition.xyz / shadowPosition.w;
    float2 shadowUV = projection.xy * float2(0.5f, -0.5f) + 0.5f;

    // インスタンスバッファの t0 は頂点シェーダ限定なので、ピクセル側の t0 をシャドウに使う。
    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f || shadowUV.y < 0.0f || shadowUV.y > 1.0f ||
        projection.z < 0.0f || projection.z > 1.0f)
    {
        return 1.0f;
    }

    // 面がライトに対して浅い角度になるほど深度誤差が出やすいので、傾きに応じてバイアスを増やす。
    float ndotl = saturate(dot(normalize(normal), -normalize(gShadowData.lightDirection)));
    float depthBias = max(0.0005f, gShadowData.bias * (1.0f - ndotl));

    // 深度シャドウを3x3 PCFで比較して、セルフシャドウのジャギーを軽減する。
    float2 texelSize = float2(1.0f / 2048.0f, 1.0f / 2048.0f) * gShadowData.pcfRadius;
    float shadow = 0.0f;
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            shadow += gShadowMap.SampleCmpLevelZero(sShadowSampler, shadowUV + float2(x, y) * texelSize, projection.z - depthBias);
        }
    }
    shadow /= 9.0f;
    return lerp(gShadowData.shadowMin, 1.0f, shadow);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;


    float4 transformedUV = mul(float4(input.texcoord.xy, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTextures[int(input.textureIndex)].Sample(sSampler, transformedUV.xy);
    //float4 textureColor = gTextures[int(input.textureIndex)].Sample(sSampler, input.texcoord);

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

        allDire *= CalculateDepthShadow(input.worldPosition, normal);




        output.color.rgb = allDire + allPoint + allSpot;


        output.color.a = gMaterial.color.a * textureColor.a * gMaterial.alpha;

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
        output.color = gMaterial.color * textureColor * gMaterial.alpha;
    }
    return output;
}
