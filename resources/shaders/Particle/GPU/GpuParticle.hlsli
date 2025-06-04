

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

float3 rand3dTo3d(float3 p)
{
    p = frac(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return frac((p.xxy + p.yzz) * p.zyx);
}

float rand3dTo1d(float3 p)
{
    p = frac(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return frac(dot((p.xxy + p.yzz) * p.zyx, float3(1.0, 1.0, 1.0)));
}

float3 hash33(float3 p)
{
    const float3x3 m =
    {
        0.00, 0.80, 0.60,
        0.60, 0.00, 0.80,
        0.80, 0.60, 0.00
    };

    p = frac(mul(p, m));
    p += dot(p, p.yzx + 19.19);
    return frac((p.xxy + p.yzz) * p.zyx);
}

float hash31(float3 p)
{
    return frac(dot(hash33(p), float3(1.0, 1.0, 1.0)));
}

class RandomGeneratetor
{
    float3 seed;
    float3 Generate3d()
    {
        seed = rand3dTo3d(seed) * 2.0f - 1.0f;
        return seed;
    }
    
    float Generate1d()
    {
        float result = rand3dTo1d(seed) * 2.0f - 1.0f;
        seed.x = result;
        return result;
    }
    
    uint counter;

    float3 Generate3d_2()
    {
        float3 offset = float3(counter, counter * 31, counter * 59);
        ++counter;
        return rand3dTo3d(seed + offset) * 2.0f - 1.0f;
    }

    float Generate1d_2()
    {
        float3 offset = float3(counter, counter * 31, counter * 59);
        ++counter;
        return rand3dTo1d(seed + offset) * 2.0f - 1.0f;
    }
};