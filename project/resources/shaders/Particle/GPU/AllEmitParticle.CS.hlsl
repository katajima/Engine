#include "EmitSpawn.hlsli"

ConstantBuffer<PerFrame> gPerFrame : register(b0); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance : register(b1); // パーティクル最大個数
ConstantBuffer<EmitterCommon> gEmitterCommon : register(b2); // エミッター(共通)
ConstantBuffer<EmitterTrail> gEmitterTrail : register(b3); // エミッター(トレイル)
ConstantBuffer<PerEmitterDispatch> gPerEmitterDispatch : register(b4); // エミッター(ディスパッチ数)





RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    
}