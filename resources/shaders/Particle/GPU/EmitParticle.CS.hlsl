#include "GpuParticle.hlsli"



ConstantBuffer<EmitterSphere> gEmitter : register(b0);  // エミッター
ConstantBuffer<PerFrame> gPerFrame : register(b1);      // 乱数生成用時間



static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticle : register(u0);


RWStructuredBuffer<int> gFreeCounter : register(u1);








[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    if (gEmitter.emit != 0)
    {
        RandomGeneratetor generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
            
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int particleIndex;
            InterlockedAdd(gFreeCounter[0],1,particleIndex);
            if (particleIndex < kMaxParticles)
            {
            // カウント分射出
                gParticle[particleIndex].scale = generator.Generate3d_2();
                gParticle[particleIndex].translate = gEmitter.translate + (generator.Generate3d());
                gParticle[particleIndex].color.rgb = generator.Generate3d_2();
                gParticle[particleIndex].color.a = 1.0f;
                gParticle[particleIndex].velocity = generator.Generate3d_2();
            }
        }
    }
}