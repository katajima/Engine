#include "Fullscreen.hlsli"


Texture2D<float4> gTexture : register(t0);
Texture2D<float> gmaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Dissovle
{
    float threshold;
    float3 color;
    float edge;
};
ConstantBuffer<Dissovle> gDissovle : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float mask = gmaskTexture.Sample(gSampler, input.texcoord);
    
    if (mask <= gDissovle.threshold)
    {
        discard;
    }
    PixelShaderOutput output;
    
    // Edge‚Á‚Û‚³‚ðŽZo
    float edge = 1.0f - smoothstep(gDissovle.threshold, gDissovle.threshold + gDissovle.edge, mask);
    output.color = gTexture.Sample(gSampler, input.texcoord);
    // Edge‚Á‚Û‚¢‚Ù‚ÇŽw’è‚µ‚½F‚ð‰ÁŽZ
    output.color.rgb += edge * gDissovle.color;
    
    return output;
}