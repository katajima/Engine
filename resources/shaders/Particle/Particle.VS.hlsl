#include"Particle.hlsli"


StructuredBuffer<ParticleGPU> gParticle : register(t1);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    
    
};

// SH 用ベクトル（定数）
static const float3 hl2_basis0 = float3(0.866f, 0.5f, 0.0f);
static const float3 hl2_basis1 = float3(-0.866f, 0.5f, 0.0f);
static const float3 hl2_basis2 = float3(0.0f, -1.0f, 0.0f);

// HL2 へ蓄積する関数
void accumulate_lighting(
    inout float3 basis_col0,
    inout float3 basis_col1,
    inout float3 basis_col2,
    float3 light_dir,
    float3 light_col,
    float atten
)
{
    light_col *= atten;

    float3 weights;
    weights.x = saturate(dot(light_dir, hl2_basis0));
    weights.y = saturate(dot(light_dir, hl2_basis1));
    weights.z = saturate(dot(light_dir, hl2_basis2));

    basis_col0 += light_col * weights.x;
    basis_col1 += light_col * weights.y;
    basis_col2 += light_col * weights.z;
}

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;

    float4 worldPos = mul(input.position, gParticle[instanceId].WVP);
    output.position = worldPos;
    output.texcoord = input.texcoord;
    
    float3 worldNormal = mul(float4(input.normal, 0.0f), (float4x4) gParticle[instanceId].World).xyz;
    output.normal = normalize(worldNormal);
    
    float3 baseColor = gParticle[instanceId].color.rgb;
    float alpha = gParticle[instanceId].color.a;

    // 初期カラーはベース
    float3 finalColor = baseColor;

    // 初期化
    output.basis_col0 = float3(0, 0, 0);
    output.basis_col1 = float3(0, 0, 0);
    output.basis_col2 = float3(0, 0, 0);

    if (gMaterial.enableLighting != 0)
    {
        float3 normal = normalize(input.normal);
        float3 lightDir = normalize(-gDirectionalLight.direction); // ライト逆向きに照らす

        // 球面調和ライティングに蓄積
        accumulate_lighting(output.basis_col0, output.basis_col1, output.basis_col2,
                            lightDir, gDirectionalLight.color.rgb * gDirectionalLight.intensity, 1.0f);

        // 重みを使って SH から復元（Diffuse ライティング再構成）
        float3 w;
        w.x = saturate(dot(normal, hl2_basis0));
        w.y = saturate(dot(normal, hl2_basis1));
        w.z = saturate(dot(normal, hl2_basis2));

        float3 light = output.basis_col0 * w.x + output.basis_col1 * w.y + output.basis_col2 * w.z;

        finalColor *= light;
    }

    output.color.rgb = finalColor;
    output.color.a = alpha;

    return output;
}

