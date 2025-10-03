#include "GpuParticle.hlsli"

struct VertexShaderOutput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};




StructuredBuffer<Particle> gParticles : register(t1);
ConstantBuffer<PerView> gPerView : register(b0);


VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    Particle particle = gParticles[instanceId];
    float4x4 worldMatrix;
    if (particle.useBillboard != 0)
    {
        worldMatrix = gPerView.billboardMatrix;
        
        worldMatrix[0] *= particle.scale.x; // Scale X   
        worldMatrix[1] *= particle.scale.y; // Scale Y
        worldMatrix[2] *= particle.scale.z; // Scale Z
        worldMatrix[3].xyz = particle.translate; // Translate
        worldMatrix[3].w = 1.0f;
    }
    else
    {
        // 通常のワールド行列を組み立てる
        float4x4 S =
        {
            particle.scale.x, 0, 0, 0,
            0, particle.scale.y, 0, 0,
            0, 0, particle.scale.z, 0,
            0, 0, 0, 1
        };

        // ここでは ZXY オイラー角回転を例に（実際は行列を外から渡すのが望ましい）
        float cx = cos(particle.rotation.x);
        float sx = sin(particle.rotation.x);
        float cy = cos(particle.rotation.y);
        float sy = sin(particle.rotation.y);
        float cz = cos(particle.rotation.z);
        float sz = sin(particle.rotation.z);

        float4x4 Rx =
        {
            1, 0, 0, 0,
            0, cx, sx, 0,
            0, -sx, cx, 0,
            0, 0, 0, 1
        };
        float4x4 Ry =
        {
            cy, 0, -sy, 0,
            0, 1, 0, 0,
            sy, 0, cy, 0,
            0, 0, 0, 1
        };
        float4x4 Rz =
        {
            cz, sz, 0, 0,
            -sz, cz, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };

        float4x4 R = mul(Rz, mul(Rx, Ry)); // 回転順序は用途に応じて調整

        float4x4 T =
        {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            particle.translate.x, particle.translate.y, particle.translate.z, 1
        };

        worldMatrix = mul(S, mul(R, T));
    }
   
    output.position = mul(input.position, mul(worldMatrix,gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;  
    return output;
}


