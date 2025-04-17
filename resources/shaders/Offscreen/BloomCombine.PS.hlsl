#include "Fullscreen.hlsli"

Texture2D<float4> gScene : register(t0);
Texture2D<float4> gBloom : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


struct BloomParams
{
    float threshold;
    float intensity;
};
ConstantBuffer<BloomParams> gBloomParams : register(b0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float3 sceneColor = gScene.Sample(gSampler, input.texcoord).rgb;
    float3 bloomColor = gBloom.Sample(gSampler, input.texcoord).rgb * gBloomParams.intensity;
    output.color = float4(sceneColor + bloomColor, 1.0);
    
    return output;
}