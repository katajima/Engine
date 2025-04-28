#include "Fullscreen.hlsli"

Texture2D<float4> gInput : register(t0);
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

float Luminance(float3 color)
{
    return dot(color, float3(0.299, 0.587, 0.114));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    
    float3 color = gInput.Sample(gSampler, input.texcoord).rgb;
    float luminance = Luminance(color);
    float mask = step(gBloomParams.threshold, luminance);
    
    output.color = float4(color * mask, 1.0f);
    
    return output;
}