#include "GpuParticle.hlsli"



static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticle : register(u0);


ConstantBuffer<PerFrame> gPerFrame : register(b0); // 乱数生成用時間


[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticle[particleIndex].color.a != 0.0f)
        {
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity;
            gParticle[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            gParticle[particleIndex].color.a = saturate(alpha);
        }
        
    }
    
    
    
    
}