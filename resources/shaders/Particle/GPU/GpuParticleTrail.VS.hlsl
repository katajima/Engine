#include "GpuParticle.hlsli"

StructuredBuffer<RibbonVertex> gRibbonVertex : register(t1);
ConstantBuffer<PerView> gPerView : register(b0);

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR;
};


VSOut main(uint id : SV_VertexID)
{
    VSOut output;
    RibbonVertex v = gRibbonVertex[id];
    
    
    if (!v.isAlive)
    {
        // 完全に画面外へ飛ばす
        output.pos = float4(0, 0, 0, 0);
        output.color = float4(0, 0, 0, 0);
        output.uv = 0;
        output.normal = 0;
        return output;
    }
    
    output.pos = mul(float4(v.pos, 1.0f), gPerView.viewProjection); // 座標変換
    output.normal = v.normal;                                       // 法線
    output.uv = v.uv;                                               // UV座標
    output.color = v.color;                                         // 頂点カラー
    
    return output;
}