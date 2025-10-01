#include "GpuParticle.hlsli"


RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
RWStructuredBuffer<int> gTrailFreeListIndex : register(u1);
RWStructuredBuffer<uint> gTrailFreeList : register(u2);

ConstantBuffer<PerFrame> gPerFrame : register(b0);              // 乱数生成用時間
ConstantBuffer<MaxInstance> gTrailMaxInstance : register(b1);   // トレイル頂点最大個数

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = DTid.x;
    if (id < gTrailMaxInstance.maxInstanse)
    {
        gRibbonVertex[id].currentTime += gPerFrame.deltaTime;
        
        if(gRibbonVertex[id].currentTime >= gRibbonVertex[id].lifeTime)
        {
            gRibbonVertex[id].lifeTime = 0.0f; // 死亡フラグ
            gRibbonVertex[id].currentTime = 0.0f;
            gRibbonVertex[id].color = float4(0,0,0,0);
            gRibbonVertex[id].pos = float3(0, 0, 0);
                         
            int freeListIndex;
            InterlockedAdd(gTrailFreeListIndex[0], 1, freeListIndex);
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
            if ((freeListIndex + 1) < gTrailMaxInstance.maxInstanse)
            {
                gTrailFreeList[freeListIndex + 1] = id;
            }
            else
            {
                // ここに来るはずがない、来たら何かが間違っているが、安全策をうっておく
                InterlockedAdd(gTrailFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}