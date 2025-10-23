#include "GpuParticle.hlsli"

RWStructuredBuffer<RibbonVertex> gRibbonVertex : register(u0);
RWStructuredBuffer<Particle> gParticle : register(u1);
ConstantBuffer<MaxInstance> gParticleMaxInstance : register(b0);
ConstantBuffer<CameraPos> gCameraPos : register(b1);

static const uint N = 5; // 必ず CPU 側定義と一致

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    if (particleIndex >= gParticleMaxInstance.maxInstance)
        return;

    Particle p = gParticle[particleIndex];

    if (p.isTrail == 0 || p.color.w <= 0.0f) // color.w = alpha
        return;

    float3 dir = normalize(p.translate - p.prevTranslate);
    float3 camDir = normalize(gCameraPos.cameraPos - p.translate);
    float3 side = cross(dir, camDir);
    if (dot(side, side) < 1e-6)
        side = float3(1, 0, 0);
    else
        side = normalize(side);

    uint head = p.trailHeadIndex;
    uint baseIndex = particleIndex * (N * 6) + head * 6; // 6頂点/Quad

    // バッファ上のオフセットが安全か確認
    // gTrailMaxInstance(max vertices) を渡すCBがあればそれを使うのがベター。
    // ここでは particleCount * N * 6 を超えないかチェックする
    // （gParticleMaxInstance と gTrailMaxInstance を両方利用しているなら gTrailMaxInstanceを使う）
    // まずは gParticleMaxInstance を用いた簡易チェック:
    uint totalVertices = gParticleMaxInstance.maxInstance * N * 6;
    if (baseIndex + 5 >= totalVertices)
    {
        // 範囲外なら安全のため書き込みをスキップしてヘッドだけ進める
        p.trailHeadIndex = (head + 1) % N;
        p.trailPrevEnd = p.translate;
        gParticle[particleIndex] = p;
        return;
    }

    // 初回: 前フレーム末端がなければ初期化だけして次へ
    if (p.hasPrevQuad == 0)
    {
        p.trailPrevEnd = p.translate;
        p.hasPrevQuad = 1;
        p.trailHeadIndex = (head + 1) % N;
        gParticle[particleIndex] = p;
        return;
    }

    float3 prevEnd = p.trailPrevEnd;

    // 頂点0..3 を作る
    RibbonVertex v0 = (RibbonVertex) 0;
    RibbonVertex v1 = (RibbonVertex) 0;
    RibbonVertex v2 = (RibbonVertex) 0;
    RibbonVertex v3 = (RibbonVertex) 0;

    v0.pos = prevEnd + side * p.trailWidth;
    v1.pos = prevEnd - side * p.trailWidth;
    v2.pos = p.translate + side * p.trailWidth;
    v3.pos = p.translate - side * p.trailWidth;

    // トレイルカラー
    float4 col = p.trailColor;
    col.w = 1.0f;
    v0.color = v1.color = v2.color = v3.color = col;

    v0.uv = float2(0, 0);
    v1.uv = float2(1, 0);
    v2.uv = float2(0, 1);
    v3.uv = float2(1, 1);

    v0.isAlive = v1.isAlive = v2.isAlive = v3.isAlive = 1;
    v0.lifeTime = v1.lifeTime = v2.lifeTime = v3.lifeTime = p.trailLifeTime;
    v0.currentTime = v1.currentTime = v2.currentTime = v3.currentTime = 0.0f;

    // 三角形展開（6頂点）
    gRibbonVertex[baseIndex + 0] = v0;
    gRibbonVertex[baseIndex + 1] = v1;
    gRibbonVertex[baseIndex + 2] = v2;

    gRibbonVertex[baseIndex + 3] = v2;
    gRibbonVertex[baseIndex + 4] = v1;
    gRibbonVertex[baseIndex + 5] = v3;

    // 頭ポインタ更新（リングバッファ）
    p.trailHeadIndex = (head + 1) % N;
    p.trailPrevEnd = p.translate;

    gParticle[particleIndex] = p;
}
