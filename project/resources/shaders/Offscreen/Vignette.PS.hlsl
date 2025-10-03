#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


struct Vignette
{
    float scale;
    float squared;
};
ConstantBuffer<Vignette> gVignette : register(b0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // 周囲を0に、中心になるほど明るくなるように計算で調整
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    
    // correctだけで計算すると中心の最大値が0.0625で明るすぎるのでScaleで調整。
    float vignette = correct.x * correct.y * gVignette.scale;
   // gVignette.scale;
    // とりあえず0.8乗でそれっぽく
    vignette = saturate(pow(vignette, gVignette.squared));
    // 係数として
    output.color.rgb *= vignette;
    return output;
}