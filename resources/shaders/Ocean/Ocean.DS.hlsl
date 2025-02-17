#include "Ocean.hlsli"

// �g�̃p�����[�^
struct WaveParameters
{
    float amplitude; // �g�̐U��
    float frequency; // �g�̎��g��
    float speed; // �g�̑��x
    float time; // ���݂̎���
};

ConstantBuffer<WaveParameters> gWaveParameters : register(b5);

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

    // �g�̉e����Z���ɉ�����iY������Ɂj
    WorldPosition.z += gWaveParameters.amplitude * (
        cos(gWaveParameters.frequency * WorldPosition.y - gWaveParameters.speed * gWaveParameters.time) +
        sin(gWaveParameters.frequency * (WorldPosition.x + WorldPosition.y) - gWaveParameters.speed * gWaveParameters.time)
    );

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

    float dZ = 1.0; // Z�������̊�i�g��Z���ɓK�p���Ă��邽�߁j

    // �������ڐ��x�N�g��
    float3 tangent = normalize(float3(1.0, dX, 0.0)); // X�����̕ω�
    float3 bitangent = normalize(float3(0.0, dY, dZ)); // Y�����̕ω�

    // �@�����v�Z�itangent �~ bitangent�j
    Output.vNormal = normalize(cross(tangent, bitangent));

    // �e�N�X�`�����W�̌v�Z
    Output.texcoord = WorldPosition.xy; // ����X, Y ���W���e�N�X�`�����W�Ɏg��

    return Output;
}
