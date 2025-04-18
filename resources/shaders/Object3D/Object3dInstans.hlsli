

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    uint textureIndex : TEXCOORD1; // インスタンスごとに異なるテクスチャID
};

struct Object3dGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
    uint textureIndex;
};