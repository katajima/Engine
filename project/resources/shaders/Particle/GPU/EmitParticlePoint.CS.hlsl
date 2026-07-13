#include "EmitSpawn.hlsli"

ConstantBuffer<EmitterPoint> gEmitter : register(b0); // エミッター(固有)
ConstantBuffer<EmitterCommon> gEmitterCommon : register(b3); // エミッター(共通)
ConstantBuffer<EmitterTrail> gEmitterTrail : register(b4); // エミッター(トレイル)
ConstantBuffer<PerEmitterDispatch> gPerEmitterDispatch : register(b5); // エミッター(ディスパッチ数)



ConstantBuffer<PerFrame> gPerFrame : register(b1); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance : register(b2); // パーティクル最大個数


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);







#define THREAD_COUNT 64
[numthreads(THREAD_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (gEmitterCommon.emit == 0)
        return;

    uint globalIndex = DTid.x;

    RandomGeneratetor generator; // 乱数生成器
    EmitSpawns emitSpawn; // エミット形状
    EmitDirections emitDirection; // エミット方向



    generator.seed = (float3(globalIndex, 0, 0) + gPerFrame.time) * gPerFrame.time;

    // スレッド数に応じて各スレッドがemit処理を分担
    for (uint countIndex = globalIndex; countIndex < gEmitterCommon.count; countIndex += gPerEmitterDispatch.totalThreadCount)
    {
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

        if (0 <= freeListIndex && freeListIndex < gMaxInstance.maxInstance)
        {
            uint particleIndex = gFreeList[freeListIndex];

            gParticle[particleIndex].translate = emitSpawn.EmitPoint(generator, gEmitterCommon, gEmitter.interpolation, countIndex, globalIndex, gPerEmitterDispatch.totalThreadCount);


            gParticle[particleIndex].velocity =
            emitDirection.EmitDirection(generator, gEmitterCommon.directionType,
            gParticle[particleIndex].translate, gEmitterCommon.translate,
            gEmitterCommon.velocity, gEmitterCommon.velocityRange, gEmitterCommon.force);

            EmitSetting_Set(generator, gParticle[particleIndex], gEmitterCommon, gEmitterTrail);




        }
        else
        {
            InterlockedAdd(gFreeListIndex[0], 1);
            break;
        }
    }
}
