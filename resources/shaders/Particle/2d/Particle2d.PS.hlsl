

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState sSampler : register(s0);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};




PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

   float4 textureColor = gTexture.Sample(sSampler, input.texcoord);
    
    

    float3 finalColor = textureColor.rgb * input.color.rgb;
    float alpha = textureColor.a * input.color.a;

    // アルファクリッピング
    if (textureColor.a <=0.5f)
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