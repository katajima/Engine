#include "GpuParticle.hlsli"


static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeCounter : register(u1);


[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    gFreeCounter[0] = 0;
    
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {    
        gParticle[particleIndex] = (Particle) 0;
        gParticle[particleIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticle[particleIndex].scale = float3(0.5f,0.5f,0.5f);
        gParticle[particleIndex].color = float4(1.0f,1.0f,1.0f,1.0f);
        gParticle[particleIndex].lifeTime = float(10.0f);
        gParticle[particleIndex].velocity = float3(0.0f, 0.0f, 0.0f);
        
    } 
}