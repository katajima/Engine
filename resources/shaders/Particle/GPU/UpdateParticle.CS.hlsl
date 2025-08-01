#include "GpuParticle.hlsli"



RWStructuredBuffer<Particle> gParticle  : register(u0);
RWStructuredBuffer<int> gFreeListIndex  : register(u1);
RWStructuredBuffer<uint> gFreeList      : register(u2);

ConstantBuffer<PerFrame> gPerFrame          : register(b0); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance    : register(b1); // パーティクル最大個数

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex < gMaxInstance.maxInstanse)
    {
        // alphaが0のparticleは死んでいるとみなして更新しない
        if (gParticle[particleIndex].color.a != 0)
        {
            gParticle[particleIndex].velocity += gParticle[particleIndex].acceleration;
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity * gPerFrame.deltaTime;
            gParticle[particleIndex].currentTime += gPerFrame.deltaTime;
            float alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            gParticle[particleIndex].color.a = saturate(alpha);
           
        }
        
        if (gParticle[particleIndex].color.a == 0)
        {
                // スケール0を入れておいてVertexShader出力で棄却されるようにする
            gParticle[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
            int freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
            if ((freeListIndex + 1) < gMaxInstance.maxInstanse)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                // ここに来るはずがない、来たら何かが間違っているが、安全策をうっておく
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}