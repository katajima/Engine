#include "GpuParticle.hlsli"

RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
ConstantBuffer<MaxInstance> gMaxInstance : register(b0); // トレイル頂点最大個数


[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{    
    uint id = DTid.x;
    if (id < gMaxInstance.maxInstanse)
    {
        gRibbonVertex[id] = (RibbonVertex) 0;
    }
}