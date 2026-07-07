

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    uint textureIndex : TEXCOORD1; // インスタンスごとに異なるテクスチャID
    float3 worldPosition : POSITION0;
    
    // 追加
    float3 tangent : TANGENT0; // 接ベクトル
    float3 biNormal : BINORMAL0; // 従ベクトル
    float3 transformedNormal : NORMAL1; // ピクセルシェーダで使用する法線
};

struct Object3dGPU
{
    float4x4 WVP;
    float4x4 World;
    float4x4 worldInverseTranspose;
    float4 color;
    uint textureIndex;
    float3 pad;
};