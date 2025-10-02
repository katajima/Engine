#include "GpuParticle.hlsli"
#include "random.hlsli"


ConstantBuffer<EmitterSphere> gEmitter : register(b0); // エミッター
ConstantBuffer<EmitterCommon> gEmitterCommon : register(b3); // エミッター




ConstantBuffer<PerFrame> gPerFrame : register(b1); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance : register(b2); // パーティクル最大個数


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

// [0,1] の乱数を generator から得る補助関数
float Random01(inout RandomGeneratetor gen)
{
    return gen.Generate1d_4() * 0.5f + 0.5f;
}

// 球体の内部に一様分布で乱数を出す（ループなし）
float3 RandomInSphere(inout RandomGeneratetor gen, float radius)
{
    // 方向を球面一様分布で作成
    float theta = Random01(gen) * 6.2831853f; // 0～2π
    float phi = acos(2.0f * Random01(gen) - 1); // 0～π

    float x = sin(phi) * cos(theta);
    float y = cos(phi);
    float z = sin(phi) * sin(theta);

    // 半径補正 pow(rand, 1/3)
    float r = radius * pow(Random01(gen), 1.0f / 3.0f);

    return float3(x, y, z) * r;
}

float3 EmitSpawn(inout RandomGeneratetor gen)
{
    if (gEmitterCommon.spawnShape == 0) // ランダム
    {
        return gEmitterCommon.translate + RandomInSphere(gen, gEmitter.radius);
    }
    else if (gEmitterCommon.spawnShape == 1) // 表面
    {
        return gEmitterCommon.translate + normalize(gen.Generate3d_4()) * gEmitter.radius;
    }
    
    return float3(0, 0, 0);
}

float3 EmitDirection(inout RandomGeneratetor gen, uint particleIndex)
{
    if (gEmitterCommon.directionType == 0) // 全方向
    {
        return gEmitterCommon.velocity + gen.Generate3d_4() * gEmitterCommon.velocityRange;
    }
    else if (gEmitterCommon.directionType == 1) // 外側
    {
        return normalize(gParticle[particleIndex].translate - gEmitterCommon.translate) * gEmitterCommon.force;
    }
    else if (gEmitterCommon.directionType == 2) // 内側
    {
        return -normalize(gParticle[particleIndex].translate - gEmitterCommon.translate) * gEmitterCommon.force;
    }
    return float3(0, 0, 0);
}



#define THREAD_COUNT 64
[numthreads(THREAD_COUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
    if (gEmitterCommon.emit == 0)
        return;

    uint globalIndex = DTid.x;

    RandomGeneratetor generator;
    generator.seed = (float3(globalIndex, 0, 0) + gPerFrame.time) * gPerFrame.time;

    // スレッド数に応じて各スレッドがemit処理を分担
    for (uint countIndex = globalIndex; countIndex < gEmitterCommon.count; countIndex += THREAD_COUNT)
    {
        int freeListIndex;
        InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);

        if (0 <= freeListIndex && freeListIndex < gMaxInstance.maxInstanse)
        {
            uint particleIndex = gFreeList[freeListIndex];

            
            gParticle[particleIndex].translate = gParticle[particleIndex].translate = EmitSpawn(generator);
            gParticle[particleIndex].prevTranslate = gParticle[particleIndex].translate;
            
            
            gParticle[particleIndex].velocity = EmitDirection(generator, particleIndex);
            
            
            
            gParticle[particleIndex].currentTime = 0;
            gParticle[particleIndex].lifeTime = gEmitterCommon.lifeTime + generator.Generate1d_4() * gEmitterCommon.lifeTimeRange;
            gParticle[particleIndex].scale = gEmitterCommon.scale + generator.Generate3d_4() * gEmitterCommon.scaleRange;
            gParticle[particleIndex].rotation = gEmitterCommon.rotate + generator.Generate3d_4() * gEmitterCommon.rotateRange;
            gParticle[particleIndex].color.rgb = gEmitterCommon.color + generator.Generate3d_4() * gEmitterCommon.colorRange;
            gParticle[particleIndex].color.a = 1.0f;
            
            gParticle[particleIndex].isAlpha = gEmitterCommon.isAlpha;
            
            gParticle[particleIndex].isScaling = gEmitterCommon.isScaling;
            gParticle[particleIndex].scaleAmount = gEmitterCommon.scaleAmount;
            
            gParticle[particleIndex].acceleration = float3(0.0f, 0.0f, 0.0f);
            
            gParticle[particleIndex].isTrail = gEmitterCommon.isTrail;
            gParticle[particleIndex].isGravity = gEmitterCommon.isGravity;

            gParticle[particleIndex].trailWidth = gEmitterCommon.trailWidth;

            gParticle[particleIndex].trailColor = gEmitterCommon.trailColor;
            
            gParticle[particleIndex].trailLifeTime = gEmitterCommon.trailLifeTime;
            
            gParticle[particleIndex].trailHeadIndex = 0;
            gParticle[particleIndex].hasPrevQuad = false;
            
            gParticle[particleIndex].isAlive = true;
         
            
            gParticle[particleIndex].useBillboard = gEmitterCommon.useBillboard;

        }
        else
        {
            InterlockedAdd(gFreeListIndex[0], 1);
            break;
        }
    }
}
