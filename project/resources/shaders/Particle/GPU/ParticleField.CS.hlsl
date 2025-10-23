#include "GpuParticle.hlsli"
#include "Collider.hlsli"


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int> gFreeListIndex : register(u1);
RWStructuredBuffer<uint> gFreeList : register(u2);

ConstantBuffer<PerFrame> gPerFrame : register(b0); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance : register(b1); // パーティクル最大個数
ConstantBuffer<EffectFieldCS> gEffectField : register(b2); // Field影響

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Collider cllider;
    
    if (gEffectField.isEffect != 0)
    {    
        uint particleIndex = DTid.x;
        if (particleIndex < gMaxInstance.maxInstance)
        {
            // alphaが0のparticleは死んでいるとみなして更新しない
            if (gParticle[particleIndex].color.a != 0)
            {
                if (cllider.CollAABBPoint(gEffectField.range, gEffectField.translate, gParticle[particleIndex].translate))
                {
                    float3 delta = gEffectField.translate - gParticle[particleIndex].translate;
                    
                    //float distSqr = max(dot(delta, delta), 0.0001); // 距離の二乗（ゼロ割防止）
                    float3 forceDir = -normalize(delta);
                    float forceMag = gEffectField.force /*/ distSqr*/; // 距離が近いほど強く、遠いほど弱く
       
                    //gParticle[particleIndex].acceleration += forceDir * forceMag * gPerFrame.deltaTime;
                    
                    gParticle[particleIndex].acceleration += forceMag * forceDir /* * gPerFrame.deltaTime*/;
                    
                }
            }
        }
    }
}