Texture2D<float4> gTexture : register(t0);
SamplerState sSampler : register(s0);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR;
};

PixelShaderOutput main(VSOut input)
{
    PixelShaderOutput output;

    float4 transformedUV = float4(input.uv.xy, 0.0f, 1.0f);
    float4 textureColor = gTexture.Sample(sSampler, transformedUV.xy);
    
    float3 finalColor = textureColor.rgb * input.color.rgb;
    float alpha = textureColor.a * input.color.a;

    // アルファクリッピング
    if (textureColor.a <= 0.5f)
    {
        discard;
    }
 
    // 完全透過なら破棄
    if (alpha <= 0.0f)
    {
        discard;
    }

    output.color = float4(finalColor, alpha);
    return output;
}