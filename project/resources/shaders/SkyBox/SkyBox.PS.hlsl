#include "SkyBox.hlsli"



struct Material
{
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);
TextureCube<float4> gTexture : register(t0);
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
    
    
    output.color = gMaterial.color * textureColor;
    
    return output;
}