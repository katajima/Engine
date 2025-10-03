#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct RadialBlur
{
    float2 Center;
    int numSamples;
    float blurWidth;
};
ConstantBuffer<RadialBlur> gRadialBlur : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    // 中心から現在のuvに対しての方向を計算。
    // 普段方向といえば、単位ベクトルだが、ここではあえて正規化せず、遠いほどより遠くをサンプリングする
    float2 direction = input.texcoord - gRadialBlur.Center;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < gRadialBlur.numSamples; ++sampleIndex)
    {
    // 現在のuvからぼかし距離にしたがってサンプリングを進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * gRadialBlur.blurWidth * float(sampleIndex);
        //texcoord = clamp(texcoord, float2(0.0f, 0.0f), float2(1.0f, 1.0f));
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    // 平均化する
    outputColor.rgb *= rcp(float(gRadialBlur.numSamples));

    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}