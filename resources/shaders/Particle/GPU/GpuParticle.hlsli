

// パーティクル
struct Particle
{
    float4 color;
    float3 translate;
    float lifeTime;
    float3 scale;
    float currentTime;
    float3 velocity;
    float pad;
};

struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
};

// エミッター(球)
struct EmitterSphere
{
    float3 translate; // 位置
    float radius; // 射出半径
    uint count; // 射出数
    float frequency; // 射出間隔
    float frequencyTime; // 射出間隔調整用時間
    uint emit; // 射出許可
};


struct PerFrame
{
    // ゲームを起動してからの時間
    float time;
    // 1フレームの経過時間
    float deltaTime;
};

// 最大個数
struct MaxInstance
{
    uint maxInstanse;
};
