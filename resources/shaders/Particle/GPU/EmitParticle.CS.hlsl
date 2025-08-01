#include "GpuParticle.hlsli"
#include "random.hlsli"


ConstantBuffer<EmitterSphere> gEmitter      : register(b0); // エミッター
ConstantBuffer<PerFrame> gPerFrame          : register(b1); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance    : register(b2); // パーティクル最大個数


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList     : register(u2);

#define THREAD_COUNT 64

[numthreads(THREAD_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    if (gEmitter.emit == 0)
        return;

    uint globalIndex = DTid.x;

    RandomGeneratetor generator;
    generator.seed = (float3(globalIndex, 0, 0) + gPerFrame.time) * gPerFrame.time;

    // スレッド数に応じて各スレッドがemit処理を分担
    for (uint countIndex = globalIndex; countIndex < gEmitter.count; countIndex += THREAD_COUNT)
    {
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

        if (0 <= freeListIndex && freeListIndex < gMaxInstance.maxInstanse)
        {
            uint particleIndex = gFreeList[freeListIndex];

            gParticle[particleIndex].currentTime = 0;
            gParticle[particleIndex].lifeTime = gEmitter.lifeTime + generator.Generate1d_4() * gEmitter.lifeTimeRange;
            gParticle[particleIndex].scale = gEmitter.scale + generator.Generate3d_4() * gEmitter.scaleRange;
            gParticle[particleIndex].translate = gEmitter.translate + generator.Generate3d_4() * gEmitter.radius;
            gParticle[particleIndex].color.rgb = gEmitter.color + generator.Generate3d_4() * gEmitter.colorRange;
            gParticle[particleIndex].color.a = 1.0f;
            gParticle[particleIndex].velocity = gEmitter.velocity + generator.Generate3d_4() * gEmitter.velocityRange;
            gParticle[particleIndex].acceleration = float3(0.0f, 0.0f, 0.0f);
        }
        else
        {
            InterlockedAdd(gFreeListIndex[0], 1);
            break;
        }
    }
}
