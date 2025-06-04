
struct EmitterSphere
{
    float3 translate;       // 位置
    float radius;           // 射出半径
    uint count;             // 射出数
    float frequency;        // 射出間隔
    float frequencyTime;    // 射出間隔調整用時間
    uint emit;              // 射出許可
};


ConstantBuffer<EmitterSphere> gEmitter : register(b0);


struct Particle
{
    float3 translate;
    float3 scale;
    float lifeTime;
    float3 velocity;
    float currentTime;
    float4 color;
};
static const uint kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticle : register(u0);


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (gEmitter.emit != 0)
    {
        for (uint countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            // カウント分射出
            gParticle[countIndex].scale = float3(0.3f, 0.3f, 0.3f);
            gParticle[countIndex].translate = float3(0.0f, 0.0f, 0.0f);
            gParticle[countIndex].color = float4(1.0f, 0.0f, 0.0f, 1.0f);
          
        }
    }
}