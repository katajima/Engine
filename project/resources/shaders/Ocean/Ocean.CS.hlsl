

// 波の頂点データ
struct Vertex
{
    float3 Position; // 頂点座標
    float3 Normal; // 法線
    float2 TexCoord; // テクスチャ座標
};

// 最大で1024分割
#define SUBDIVISION 16  // (例: 16x16 = 256分割)

// 頂点バッファ用の構造化バッファ
RWStructuredBuffer<Vertex> vertexBuffer : register(u0);

// メッシュの分割計算
[numthreads(8, 8, 1)] // 8×8 スレッド (合計64スレッド)
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    // Compute Shader のスレッドIDを取得
    uint x = DTid.x;
    uint y = DTid.y;

    // メッシュの細かさを決定
    float u = float(x) / (SUBDIVISION - 1);
    float v = float(y) / (SUBDIVISION - 1);

    // ベースとなる四角形の頂点を計算
    float3 worldPos = float3(u * 10.0 - 5.0, 0.0, v * 10.0 - 5.0);

    // 波の影響を計算
    float height = sin(worldPos.x * 2.0 + worldPos.z * 2.0);
    worldPos.y += height * 0.5; // 波の高さを適用

    // 法線の計算 (簡易版)
    float3 normal = normalize(float3(-cos(worldPos.x * 2.0), 1.0, -cos(worldPos.z * 2.0)));

    // 頂点データをバッファに格納
    uint index = y * SUBDIVISION + x;
    vertexBuffer[index].Position = worldPos;
    vertexBuffer[index].Normal = normal;
    vertexBuffer[index].TexCoord = float2(u, v);
}
