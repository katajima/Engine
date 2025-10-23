#include "GpuParticle.hlsli"

RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
ConstantBuffer<PerFrame> gPerFrame : register(b0);
ConstantBuffer<MaxInstance> gTrailMaxInstance : register(b1);

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    if (id >= gTrailMaxInstance.maxInstance)
        return;

    RibbonVertex v = gRibbonVertex[id];
    // 未使用なら処理しない（不要な書き込みを抑える）
    if (v.isAlive == 0)
        return;
    v.currentTime += gPerFrame.deltaTime;

    if (v.currentTime >= v.lifeTime)
    {
        // 無効化して早期抜け（画面描画側で無視されるようにする）
        v.isAlive = 0;
        v.color = float4(0, 0, 0, 0);
        v.pos = float3(0, 0, 0);
        v.currentTime = 0.0f;
        v.lifeTime = 0.0f;
    }

    gRibbonVertex[id] = v;
}
