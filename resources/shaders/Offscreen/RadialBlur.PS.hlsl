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
    // ���S���猻�݂�uv�ɑ΂��Ă̕������v�Z�B
    // ���i�����Ƃ����΁A�P�ʃx�N�g�������A�����ł͂����Đ��K�������A�����قǂ�艓�����T���v�����O����
    float2 direction = input.texcoord - gRadialBlur.Center;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int sampleIndex = 0; sampleIndex < gRadialBlur.numSamples; ++sampleIndex)
    {
    // ���݂�uv����ڂ��������ɂ��������ăT���v�����O��i�߂Ȃ���T���v�����O���Ă���
        float2 texcoord = input.texcoord + direction * gRadialBlur.blurWidth * float(sampleIndex);
        //texcoord = clamp(texcoord, float2(0.0f, 0.0f), float2(1.0f, 1.0f));
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    // ���ω�����
    outputColor.rgb *= rcp(float(gRadialBlur.numSamples));

    PixelShaderOutput output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}