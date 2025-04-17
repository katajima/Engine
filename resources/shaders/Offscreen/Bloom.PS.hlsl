#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Bloom
{
    float2 texelSize; // 1 / �e�N�X�`���̉𑜓x
    float blurRadius; // �u���[���a�i��: 5�j
};

ConstantBuffer<Bloom> gBloom : register(b0);






PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color.a = 1.0f;
    float weights[5] = { 0.204164, 0.304005, 0.093913, 0.015628, 0.001015 }; // �Ώ̂ȃK�E�X�W��    
    float3 color = gTexture.Sample(gSampler, input.texcoord).rgb * weights[0];

    for (int i = 1; i < 5; ++i)
    {
        float offset = i * gBloom.blurRadius;
        color += gTexture.Sample(gSampler, input.texcoord + float2(offset, 0) * gBloom.texelSize).rgb * weights[i];
        color += gTexture.Sample(gSampler, input.texcoord - float2(offset, 0) * gBloom.texelSize).rgb * weights[i];
    }
    output.color.rgb = color;
    return output;
}