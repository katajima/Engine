#include "GpuParticle.hlsli"


RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
RWStructuredBuffer<int> gTrailFreeListIndex : register(u1);
RWStructuredBuffer<uint> gTrailFreeList : register(u2);
ConstantBuffer<MaxInstance> gTrailMaxInstance : register(b0); // トレイル頂点最大個数

RWStructuredBuffer<Particle> gParticle : register(u3);
ConstantBuffer<MaxInstance> gParticleMaxInstance : register(b1); // パーティクル最大個数
ConstantBuffer<CameraPos> gCameraPos : register(b2); // カメラ位置
// エミッタ
[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    if (particleIndex < gParticleMaxInstance.maxInstanse)
    {
        Particle p = gParticle[particleIndex];
        
        if (p.color.a > 0 && p.isTrail == true)
        {
            float3 dir = normalize(p.translate - p.prevTranslate);
            
             // カメラ方向
            float3 camDir = normalize(gCameraPos.cameraPos - p.translate);

            // リボンの横方向（進行方向×カメラ方向）
            float3 side = normalize(cross(dir, camDir));
            
            RibbonVertex v0, v1, v2, v3;
            v0.pos = p.prevTranslate + side * p.trailWidth;
            v1.pos = p.prevTranslate - side * p.trailWidth;
            v2.pos = p.translate + side * p.trailWidth;
            v3.pos = p.translate - side * p.trailWidth;
            
            v0.color = p.color;
            v1.color = p.color;
            v2.color = p.color;
            v3.color = p.color;
            
            // UV （例: リボンの両端 0,1 割り当て）
            v0.uv = float2(0.0f, 0.0f);
            v1.uv = float2(1.0f, 0.0f);
            v2.uv = float2(0.0f, 1.0f);
            v3.uv = float2(1.0f, 1.0f);
            
            v0.normal = float3(0, 0, 0);
            v1.normal = float3(0, 0, 0);
            v2.normal = float3(0, 0, 0);
            v3.normal = float3(0, 0, 0);
            
            v0.lifeTime = p.trailLifeTime;
            v1.lifeTime = p.trailLifeTime;
            v2.lifeTime = p.trailLifeTime;
            v3.lifeTime = p.trailLifeTime;
            
            v0.currentTime = 0.0f;
            v1.currentTime = 0.0f;
            v2.currentTime = 0.0f;
            v3.currentTime = 0.0f;
            
            
            v0.pad = 0.0f;
            v1.pad = 0.0f;
            v2.pad = 0.0f;
            v3.pad = 0.0f;
            
            
            v0.isAlive = true;
            v1.isAlive = true;
            v2.isAlive = true;
            v3.isAlive = true;
            
            
            
            int freeListIndex;
            InterlockedAdd(gTrailFreeListIndex[0], -4, freeListIndex);

            if (freeListIndex >= 0 && freeListIndex < gTrailMaxInstance.maxInstanse)
            {
                uint baseIndex = gTrailFreeList[freeListIndex];

                gRibbonVertex[baseIndex + 0] = v0;
                gRibbonVertex[baseIndex + 1] = v1;
                gRibbonVertex[baseIndex + 2] = v2;
                gRibbonVertex[baseIndex + 3] = v3;
            }
            else
            {
                // FreeListを戻す（失敗時）
                InterlockedAdd(gTrailFreeListIndex[0], 4);
            }
     
        }
    }
}