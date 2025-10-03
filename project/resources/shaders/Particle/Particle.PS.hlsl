#include"Particle.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState sSampler : register(s0);

ConstantBuffer<ParticleMaterial> material : register(b0);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};



// HL2 用の基底ベクトル（定数）
static const float3 hl2_basis0 = float3(0.866f, 0.5f, 0.0f);
static const float3 hl2_basis1 = float3(-0.866f, 0.5f, 0.0f);
static const float3 hl2_basis2 = float3(0.0f, -1.0f, 0.0f);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 transformedUV = mul(float4(input.texcoord.xy, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(sSampler, transformedUV.xy);
    
    

    float3 finalColor = textureColor.rgb * input.color.rgb;
    float alpha = textureColor.a * input.color.a;

    // アルファクリッピング
    if (textureColor.a <= gMaterial.alphaClipping)
    {
        discard;
    }

    if (gMaterial.enableLighting != 0)
    {
    // HL2 ライティング再構成
        float3 normal = normalize(input.normal);

        float3 w;
        w.x = saturate(dot(normal, hl2_basis0));
        w.y = saturate(dot(normal, hl2_basis1));
        w.z = saturate(dot(normal, hl2_basis2));

        float3 light = input.basis_col0 * w.x +
                   input.basis_col1 * w.y +
                   input.basis_col2 * w.z;
        finalColor *= light;
    }
    
    // 完全透過なら破棄
    if (alpha <= 0.0f)
    {
        discard;
    }

    output.color = float4(finalColor, alpha);
    return output;
}