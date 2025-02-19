#include "Ocean.hlsli"
// �g�̃p�����[�^
struct WaveParameters
{
    float2 waveDirection; // �g�̐i�s�����i�P�ʃx�N�g���j
    float amplitude; // �g�̐U��
    float frequency; // �g�̎��g��
    float speed; // �g�̑��x
    float time; // ���݂̎���
   
};
ConstantBuffer<WaveParameters> gWaveParameters : register(b5);


// �m�C�Y�̃p�����[�^
struct NoiseParameters
{
    float noiseScale; // �m�C�Y�̃X�P�[��
    float noiseStrength; // �m�C�Y�̋��x
    int octaves; // �t���N�^���m�C�Y�̃I�N�^�[�u��
    float roughness; // �e�I�N�^�[�u�̉e���x 
};
ConstantBuffer<NoiseParameters> gNoiseParameters : register(b6);



// --- �[���m�C�Y�֐��i�n�b�V���x�[�X�j ---
float Hash(float2 p)
{
    float3 p3 = frac(float3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 19.19);
    return frac((p3.x + p3.y) * p3.z);
}

// --- Perlin���m�C�Y ---
float PerlinNoise(float2 uv)
{
    float2 i = floor(uv);
    float2 f = frac(uv);

    float a = Hash(i);
    float b = Hash(i + float2(1.0, 0.0));
    float c = Hash(i + float2(0.0, 1.0));
    float d = Hash(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f); // �X���[�W���O

    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

// --- �t���N�^���m�C�Y�iFbm�j ---
float FractalNoise(float2 uv)
{
    float amplitude = 1.0;
    float total = 0.0;
    float maxValue = 0.0; // ���K���p
    float frequency = 1.0;

    for (int i = 0; i < gNoiseParameters.octaves; i++)
    {
        total += PerlinNoise(uv * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= gNoiseParameters.roughness; // ���t�l�X�ŐU���𒲐�
        frequency *= 2.0; // ���g����{��
    }

    return total / maxValue; // ���K��
}

[domain("quad")]
DS_OUTPUT main(
    HS_CONSTANT_DATA_OUTPUT input,
    float2 domain : SV_DomainLocation, // x, y��2�̃o���Z���g���b�N���W
    const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch) // 4����_
{
    DS_OUTPUT Output;

    // �l�ӌ`�̃o���Z���g���b�N���W���g���Ĉʒu����
    float3 WorldPosition =
        patch[0].vPosition * (1.0f - domain.x - domain.y + domain.x * domain.y) + // ����
        patch[1].vPosition * domain.x * (1.0f - domain.y) + // �E��
        patch[3].vPosition * domain.x * domain.y + // �E��
        patch[2].vPosition * (1.0f - domain.x) * domain.y; // ����

    // --- �ʏ�̔g + �t���N�^���m�C�Y ---
    float wavePhase = dot(WorldPosition.xy, gWaveParameters.waveDirection);
    float wave = gWaveParameters.amplitude * (
    cos(gWaveParameters.frequency * (wavePhase) - gWaveParameters.speed * gWaveParameters.time) +
    sin(gWaveParameters.frequency * (wavePhase + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time));

    // �t���N�^���m�C�Y��ǉ�
    float noise = gNoiseParameters.noiseStrength * FractalNoise(WorldPosition.xy * gNoiseParameters.noiseScale + gWaveParameters.time);

    // Z���ɓK�p
    WorldPosition.z += wave + noise;

    // ���[���h���W��ۑ�
    Output.vWorldPos = WorldPosition;

    // �X�N���[�����W�ɕϊ�
    Output.vPosition = mul(float4(WorldPosition, 1.0), gTransformationMatrix.WVP);

    // --- �@���̍Čv�Z ---
    float dX = -gWaveParameters.amplitude * gWaveParameters.frequency *
               sin(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time);
    
    float dY = -gWaveParameters.amplitude * gWaveParameters.frequency * (
               sin(gWaveParameters.frequency * WorldPosition.y - gWaveParameters.speed * gWaveParameters.time) +
               cos(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time)
    );

    // �t���N�^���m�C�Y�̌��z��ǉ�
    float noiseDX = gNoiseParameters.noiseStrength * (FractalNoise((WorldPosition.xy + float2(0.01, 0)) * gNoiseParameters.noiseScale) - noise);
    float noiseDY = gNoiseParameters.noiseStrength * (FractalNoise((WorldPosition.xy + float2(0, 0.01)) * gNoiseParameters.noiseScale) - noise);

    float dZ = 1.0; // Z�������̊�i�g��Z���ɓK�p���Ă��邽�߁j

    // �������ڐ��x�N�g��
    float3 tangent = normalize(float3(1.0, dX + noiseDX, 0.0)); // X�����̕ω�
    float3 bitangent = normalize(float3(0.0, dY + noiseDY, dZ)); // Y�����̕ω�

    // �@�����v�Z�itangent �~ bitangent�j
    Output.vNormal = normalize(cross(tangent, bitangent));

    // �e�N�X�`�����W�̌v�Z
    Output.texcoord = WorldPosition.xy; // ����X, Y ���W���e�N�X�`�����W�Ɏg��

    return Output;
}
