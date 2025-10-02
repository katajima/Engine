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
void main(uint3 DTid : SV_DispatchThreadID)
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
            float3 side = cross(dir, camDir);
            if (dot(side, side) < 1e-6)
                side = float3(1, 0, 0); // fallback
            else
                side = normalize(side);
            
            // Quadの4頂点
            RibbonVertex v0, v1, v2, v3;
            v0.pos = p.prevTranslate + side * p.trailWidth;
            v1.pos = p.prevTranslate - side * p.trailWidth;
            v2.pos = p.translate + side * p.trailWidth;
            v3.pos = p.translate - side * p.trailWidth;
            
            v0.color = v1.color = v2.color = v3.color = p.color;
            
            // UV （リボンの両端 0,1 割り当て）
            v0.uv = float2(0.0f, 0.0f);
            v1.uv = float2(1.0f, 0.0f);
            v2.uv = float2(0.0f, 1.0f);
            v3.uv = float2(1.0f, 1.0f);

            v0.normal = v1.normal = v2.normal = v3.normal = float3(0, 0, 0);

            v0.lifeTime = v1.lifeTime = v2.lifeTime = v3.lifeTime = p.trailLifeTime;
            v0.currentTime = v1.currentTime = v2.currentTime = v3.currentTime = 0.0f;

            v0.pad = v1.pad = v2.pad = v3.pad = 0.0f;

            v0.isAlive = v1.isAlive = v2.isAlive = v3.isAlive = true;

            // --- ここから6頂点展開 ---
            // 三角形1 (v0,v1,v2)
            // 三角形2 (v2,v1,v3)
            int freeListIndex;
            InterlockedAdd(gTrailFreeListIndex[0], -6, freeListIndex);

            if (freeListIndex >= 0 && (freeListIndex + 5) < gTrailMaxInstance.maxInstanse)
            {
                uint baseIndex = gTrailFreeList[freeListIndex];

                gRibbonVertex[baseIndex + 0] = v0;
                gRibbonVertex[baseIndex + 1] = v1;
                gRibbonVertex[baseIndex + 2] = v2;

                gRibbonVertex[baseIndex + 3] = v2;
                gRibbonVertex[baseIndex + 4] = v1;
                gRibbonVertex[baseIndex + 5] = v3;
            }
            else
            {
                // FreeListを戻す（失敗時）
                InterlockedAdd(gTrailFreeListIndex[0], 6);
            }
        }
    }
}