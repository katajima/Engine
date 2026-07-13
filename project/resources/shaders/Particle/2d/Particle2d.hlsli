struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

struct ParticleGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};


struct Material
{

    float4 color;
    int enableLighting;
    float alphaClipping;
    float environmentCoefficient;
    float alpha;
    float4x4 uvTransform;
    float shininess;
    int useLig;
    int useHem;
    int useNormalMap;
    int useSpecularMap;
};
ConstantBuffer<Material> gMaterial : register(b0);