#include "EmitSpawn.hlsli"

StructuredBuffer<EmitterCommon> gEmitters : register(t0); // エミッター
StructuredBuffer<EmitterTrail> gEmitterTrail : register(t1); // エミッター(トレイル)
StructuredBuffer<PerEmitterDispatch> gEmitterDispatch : register(t2);

RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

ConstantBuffer<PerFrame> gPerFrame : register(b0);
ConstantBuffer<MaxInstance> gMaxInstance : register(b1);
ConstantBuffer<DispatchCount> gDispatchCount : register(b2);


#define THREAD_COUNT 64
[numthreads(THREAD_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint globalThread = DTid.x;

    // --- まず自分が担当する PerEmitterDispatch を見つける ---
    bool found = false;
    PerEmitterDispatch myDispatch;
    for (uint d = 0; d < gDispatchCount.gEmitterDispatchCount; ++d)
    {
        PerEmitterDispatch info = gEmitterDispatch[d];
        uint start = info.startThread;
        uint end = info.startThread + info.totalThreadCount;
        if (globalThread >= start && globalThread < end)
        {
            myDispatch = info;
            found = true;
            break;
        }
    }

    if (!found)
    {
        // 自分に割り当てが無ければ何もしない
        return;
    }

    // --- エミッタ本体を取り出す ---
    uint emitterID = myDispatch.emitterIndex;
    EmitterCommon emitter = gEmitters[emitterID];
    EmitterTrail emitterTrail = gEmitterTrail[emitterID];

    if (emitter.emit == 0)
        return;

    // --- ローカルスレッドID と 書き込みベースを決定 ---
    uint localThreadIndex = globalThread - myDispatch.startThread; // 0 .. myDispatch.totalThreadCount-1
    uint particleBase = myDispatch.particleOffset; // （今回は使わない：freeListから返るIndexを使う）

    // --- 乱数ジェネレータ初期化 ---
    RandomGeneratetor generator;
    generator.seed = (float3(globalThread, emitterID, 0) + gPerFrame.time) * gPerFrame.time;

    EmitSpawns emitSpawn;
    EmitDirections emitDir;

    // --- emit のループ（stride = myDispatch.totalThreadCount） ---
    uint stride = myDispatch.totalThreadCount;
    for (uint emitIndex = localThreadIndex; emitIndex < emitter.count; emitIndex += stride)
    {
        int freeIdx;
        InterlockedAdd(gFreeListIndex[0], -1, freeIdx);

        if (freeIdx < 0 || freeIdx >= gMaxInstance.maxInstance)
        {
            // 取り出し失敗したら戻して終わり
            InterlockedAdd(gFreeListIndex[0], 1);
            break;
        }

        uint particleIndex = gFreeList[freeIdx]; // 実際の書き込み先（グローバルパーティクルインデックス）

        // --- 形状に応じて射出座標算出 ---
        float3 pos = 0;
        switch (emitter.shapeType)
        {
            case 0: // AABB
                pos = emitSpawn.EmitAABB(generator, emitter.spawnShape, emitter.translate, emitter.size, particleIndex);
                break;
            case 1: // Sphere
                pos = emitSpawn.EmitSphere(generator, emitter.spawnShape, emitter.translate, emitter.sphereRadius);
                break;
            default:
                pos = emitter.translate;
                break;
        }

        // --- 方向・速度 ---
        float3 vel = emitDir.EmitDirection(generator, emitter.directionType, pos, emitter.translate,
                                           emitter.velocity, emitter.velocityRange, emitter.force);

        // --- パーティクル初期化 ---
        Particle p = (Particle) 0; // 全要素ゼロクリア
        p.translate = pos;
        p.velocity = vel;
        EmitSetting_Set(generator, p, emitter, emitterTrail);

        // --- 書き込みは free list が返すインデックスへ ---
        gParticle[particleIndex] = p;
    }
}
