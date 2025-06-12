#include "GpuParticle.hlsli"



RWStructuredBuffer<Particle> gParticle   : register(u0);
RWStructuredBuffer<int> gFreeListIndex   : register(u1);
RWStructuredBuffer<uint> gFreeList       : register(u2);

ConstantBuffer<MaxInstance> gMaxInstance : register(b0); // パーティクル最大個数

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    gFreeListIndex[0] = 0;
    
    uint particleIndex = DTid.x;
    if (particleIndex < gMaxInstance.maxInstanse)
    {    
        gParticle[particleIndex] = (Particle) 0;
        gParticle[particleIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticle[particleIndex].scale = float3(0.5f,0.5f,0.5f);
        gParticle[particleIndex].color = float4(1.0f,1.0f,1.0f,1.0f);
        gParticle[particleIndex].lifeTime = float(0.0f);
        gParticle[particleIndex].velocity = float3(0.0f, 0.0f, 0.0f);
        gParticle[particleIndex].acceleration = float3(0.0f, 0.0f, 0.0f);
        
        gParticle[particleIndex].currentTime = float(0.0f);
        
        gFreeList[particleIndex] = particleIndex;
        
        if (particleIndex == 0)
        {
            gFreeListIndex[0] = gMaxInstance.maxInstanse - 1;
        }
        
    } 
}