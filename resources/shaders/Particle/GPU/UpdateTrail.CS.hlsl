#include "GpuParticle.hlsli"

RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
RWStructuredBuffer<int> gTrailFreeListIndex : register(u1);
RWStructuredBuffer<uint> gTrailFreeList : register(u2);

ConstantBuffer<PerFrame> gPerFrame : register(b0); // 乱数生成用時間
ConstantBuffer<MaxInstance> gTrailMaxInstance : register(b1); // トレイル頂点最大個数

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint id = DTid.x;
    if (id >= gTrailMaxInstance.maxInstanse)
        return;

    // バッファから頂点取得
    RibbonVertex v = gRibbonVertex[id];

    // 生存していない頂点は更新しない
    //if (!v.isAlive)
     //   return;

    // 時間更新
    v.currentTime += gPerFrame.deltaTime;

    // 寿命チェック
    if (v.currentTime >= v.lifeTime)
    {
        // 死亡フラグ
        v.isAlive = false;
        v.color = float4(0, 0, 0, 0);
        v.pos = float3(0, 0, 0);
        v.currentTime = 0.0f;
        v.lifeTime = 0.0f;

        // FreeList に戻す
        int freeListIndex;
        InterlockedAdd(gTrailFreeListIndex[0], 1, freeListIndex);

        // +1 して格納
        if ((freeListIndex + 1) < gTrailMaxInstance.maxInstanse)
        {
            gTrailFreeList[freeListIndex + 1] = id;
        }
        else
        {
            // 想定外 → カウンタを戻す
            InterlockedAdd(gTrailFreeListIndex[0], -1, freeListIndex);
        }
    }

    // UAV に書き戻し
    gRibbonVertex[id] = v;
}
