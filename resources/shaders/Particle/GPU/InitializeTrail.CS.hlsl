#include "GpuParticle.hlsli"

RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

ConstantBuffer<MaxInstance> gMaxInstance : register(b0); // トレイル頂点最大個数


[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    gFreeListIndex[0] = 0;
    
    uint id = DTid.x;
    if (id < gMaxInstance.maxInstanse)
    {
        gRibbonVertex[id] = (RibbonVertex) 0;
        
        gRibbonVertex[id].color = float4(0.0f, 0.0f, 0.0f, 0.0f);
        gRibbonVertex[id].currentTime = float(0.0f);
        gRibbonVertex[id].lifeTime = float(0.0f);
        gRibbonVertex[id].pos = float3(0.0f, 0.0f, 0.0f);
        gRibbonVertex[id].isAlive = false;
        gRibbonVertex[id].normal = float3(0.0f, 0.0f, 0.0f);
        gRibbonVertex[id].uv = float2(0.0f, 0.0f);
        gRibbonVertex[id].pad = 0;
        
        
        gFreeList[id] = id;
        
        if (id == 0)
        {
            gFreeListIndex[0] = gMaxInstance.maxInstanse - 1;
        }
        
    }
}