#include "GpuParticle.hlsli"



RWStructuredBuffer<Particle> gParticle  : register(u0);
RWStructuredBuffer<int> gFreeListIndex  : register(u1);
RWStructuredBuffer<uint> gFreeList      : register(u2);

ConstantBuffer<PerFrame> gPerFrame              : register(b0); // 乱数生成用時間
ConstantBuffer<MaxInstance> gMaxInstance        : register(b1); // パーティクル最大個数
ConstantBuffer<DeleteParticle> gDeleteParticle  : register(b2); // パーティクル削除


[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    
    
    if (particleIndex < gMaxInstance.maxInstance)
    {
         
        if (gParticle[particleIndex].currentTime <= gParticle[particleIndex].lifeTime)
        {
            // 削除フラグが立っていたら消す
            if (gDeleteParticle.isDelete != 0)
            {
                // スケール0を入れておいてVertexShader出力で棄却されるようにする
                gParticle[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                gParticle[particleIndex].color.a = 0.0f;
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
                if ((freeListIndex + 1) < gMaxInstance.maxInstance)
                {
                    gFreeList[freeListIndex + 1] = particleIndex;
                }
                else
                {
                // ここに来るはずがない、来たら何かが間違っているが、安全策をうっておく
                    InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                }
                return;
            }
    
            
            
            gParticle[particleIndex].prevTranslate = gParticle[particleIndex].translate;
          
            
            gParticle[particleIndex].velocity += gParticle[particleIndex].acceleration;
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity * gPerFrame.deltaTime;
            gParticle[particleIndex].currentTime += gPerFrame.deltaTime;
            
            // 透けるようにする
            if (gParticle[particleIndex].isAlpha == true)
            {
                float alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
                gParticle[particleIndex].color.a = saturate(alpha);
            }
            
            // スケーリングする
            if (gParticle[particleIndex].isScaling == true)
            {
                float scale = gParticle[particleIndex].scaleAmount * gPerFrame.deltaTime;
                
                gParticle[particleIndex].scale.x += scale;
                gParticle[particleIndex].scale.y += scale;
                gParticle[particleIndex].scale.z += scale;
            }
            
            // 重力を加える
            if (gParticle[particleIndex].isGravity)
            {
                gParticle[particleIndex].acceleration.y -= 9.8f * gPerFrame.deltaTime; // 重力加速度
            }
            
            
            if (length(gParticle[particleIndex].scale) < 0.0f)
            {  
                gParticle[particleIndex].color.a = 0.0f;
            }
           
            
            if (gParticle[particleIndex].currentTime > gParticle[particleIndex].lifeTime)
            {
                // スケール0を入れておいてVertexShader出力で棄却されるようにする
                gParticle[particleIndex].scale = float3(0.0f, 0.0f, 0.0f);
                gParticle[particleIndex].color.a = 0.0f;
                int freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                // 最新のFreeListIndexの場所に死んだParticleのIndexを設定する
                if ((freeListIndex + 1) < gMaxInstance.maxInstance)
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
}