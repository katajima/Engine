#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Random
{
    float time;
};
ConstantBuffer<Random> gRandom : register(b0);

float rand2dTo1d(float2 uv, float seed)
{
    float2 r = float2(
        dot(uv, float2(127.1, 311.7)),
        dot(uv, float2(269.5, 183.3))
    );

    return frac(sin(dot(r, float2(12.9898, 78.233))) * 43758.5453 + seed);
}


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float random = rand2dTo1d(input.texcoord,gRandom.time);
    
    float r = rand2dTo1d(input.texcoord + float2(0.1, 0.2), gRandom.time);
    float g = rand2dTo1d(input.texcoord + float2(0.3, 0.4), gRandom.time);
    float b = rand2dTo1d(input.texcoord + float2(0.5, 0.6), gRandom.time);
    output.color *= float4(r, g, b, 1.0f);
    
    return output;
}