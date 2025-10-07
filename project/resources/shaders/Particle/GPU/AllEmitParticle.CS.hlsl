#include "EmitSpawn.hlsli"

ConstantBuffer<EmitterSphere> gEmitter : register(b0); // エミッター(固有)
ConstantBuffer<PerFrame> gPerFrame : register(b1); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance : register(b2); // パーティクル最大個数
ConstantBuffer<EmitterCommon> gEmitterCommon : register(b3); // エミッター(共通)
ConstantBuffer<EmitterTrail> gEmitterTrail : register(b4); // エミッター(トレイル)
ConstantBuffer<PerEmitterDispatch> gPerEmitterDispatch : register(b5); // エミッター(ディスパッチ数)





RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);






[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}