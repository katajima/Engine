
struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
    
    float3 basis_col0 : TEXCOORD1;
    float3 basis_col1 : TEXCOORD2;
    float3 basis_col2 : TEXCOORD3;
    float3 normal : NORMAL0; // ←必要！
};

struct ParticleGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

struct ParticleMaterial
{
    float alphaClipping;
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
    int useSpeculerMap;
};
ConstantBuffer<Material> gMaterial : register(b0);

// 平行光線
struct DirectionalLight
{
    float4 color; //!< ライトの色
    float3 direction; //!< ライトの向き
    float intensity; //!< 輝度
    float ilg; // リグ
    int enableLighting;
    float3 groundColor; // 地面色
    float3 skyColor; // 天球色
    float3 groundNormal; // 地面法線方向
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
