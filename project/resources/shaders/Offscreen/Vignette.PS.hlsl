#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


struct Vignette
{
    float4 color;
    float scale;
    float squared;
};
ConstantBuffer<Vignette> gVignette : register(b0);



PixelShaderOutput main(VertexShaderOutput input)
{
    // 元画像を取得し、中心部に色が適用されないように保持する。
    PixelShaderOutput output;
    float4 sourceColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 周囲を0に、中心になるほど明るくなるように計算で調整
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    
    // 画面中心が1、画面端が0になるVignetteの強度を求める。
    float vignette = correct.x * correct.y * gVignette.scale;
    vignette = saturate(pow(vignette, gVignette.squared));

    // Vignetteの強度を反転し、周辺部だけに色を適用するマスクにする。
    // 中心部は元画像、周辺部は指定色へブレンドする。
    output.color.rgb = lerp(sourceColor.rgb, gVignette.color.rgb, 1.0f - vignette);
    output.color.a = sourceColor.a;
    return output;
}
