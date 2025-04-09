#include"Object3dInstans.hlsli"

Texture2D<float4> gTextures[] : register(t1);
SamplerState sSampler : register(s0);

////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
  
    float4 textureColor = gTextures[int(input.textureIndex)].Sample(sSampler, input.texcoord);
    output.color = textureColor * input.color;
    
    if (textureColor.a <= 0.5f || output.color.a <= 0.0f)
    {
        discard;
    }

    
    
    return output;
}