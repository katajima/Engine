
struct LineGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
    float3 strPos;
    float3 endPos;
};
struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};