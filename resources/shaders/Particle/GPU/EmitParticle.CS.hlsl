#include "GpuParticle.hlsli"
#include "random.hlsli"


ConstantBuffer<EmitterSphere> gEmitter      : register(b0); // エミッター
ConstantBuffer<PerFrame> gPerFrame          : register(b1); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance    : register(b2); // パーティクル最大個数


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList     : register(u2);








[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    if (gEmitter.emit != 0)
    {
        RandomGeneratetor generator;
            
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
            
            // FreeListのIndexをひとつ前に設定し、現在のIndexを取得する
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            if (0 <= freeListIndex && freeListIndex < gMaxInstance.maxInstanse)
            {
                int particleIndex = gFreeList[freeListIndex];
                gParticle[particleIndex].currentTime = 0;
                gParticle[particleIndex].lifeTime = 30.0f;
                gParticle[particleIndex].scale = generator.Generate3d_4();
                gParticle[particleIndex].translate = gEmitter.translate + generator.Generate3d_4();
                gParticle[particleIndex].color.rgb = generator.Generate3d_4();
                gParticle[particleIndex].color.a = 1.0f;
                gParticle[particleIndex].velocity = generator.Generate3d_4() * 0.5f;
            }
            else
            {
                // 発生させられなかったので、減らしてしまった分もとに戻す。これを忘れると発生させられなかった分だけどんどんIndexが減ってしまう
                InterlockedAdd(gFreeListIndex[0], 1);
                // Emit中にParticleは消えないので、この後発生することはないためbreakしておわる
                break;
            }
        }
    }
}