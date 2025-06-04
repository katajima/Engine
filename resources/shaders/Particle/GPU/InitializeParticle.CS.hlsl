
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

[numthreads(1024, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {    
        gParticle[particleIndex] = (Particle) 0;
        gParticle[particleIndex].translate = float3(0.0f, 0.0f, 0.0f);
        gParticle[particleIndex].scale = float3(1.5f,1.5f,1.5f);
        gParticle[particleIndex].color = float4(1.0f,1.0f,1.0f,1.0f);   
    } 
}