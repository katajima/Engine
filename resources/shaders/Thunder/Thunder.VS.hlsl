#include "Thunder.hlsli"

// �����_���l�𐶐�����֐�
float randomOffset(float2 uv, float scale)
{
    float rand = frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    return (rand - 0.5) * scale; // �X�P�[������ (-scale/2 �` scale/2)
}

// ���̒��S����̋��x���v�Z����֐�
float computeIntensity(float3 position, float centerX, float falloff)
{
    return saturate(1.0 - abs(position.x - centerX) * falloff);
}

// ���_�V�F�[�_�[�֐�
ShaderIO VertexShaderFunction(float3 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
    ShaderIO output;

    // ���[���h��ԍ��W�ւ̕ϊ�
    float4 worldPosition = mul(float4(position, 1.0), gTransformationMatrix.World);

    // �����_���ȃI�t�Z�b�g��K�p
    float randomOffsetValue = randomOffset(texcoord, 0.1); // ��: �X�P�[�� 0.1
    worldPosition.x += randomOffsetValue;

    // �X�N���[�����W�ϊ�
    float4 worldViewProjPosition = mul(worldPosition, gTransformationMatrix.WVP);

    // �@���x�N�g���̕ϊ� (���[���h���)
    float3 transformedNormal = mul(float4(normal, 0.0), gTransformationMatrix.WorldInverseTranspose).xyz;

    // ���̒��S����̋��x���v�Z (���S�� x=0�A�����W�� 1.0 �Ɖ���)
    float intensity = computeIntensity(worldPosition.xyz, 0.0, 1.0);

    // �o�̓f�[�^�̐ݒ�
    output.position = worldViewProjPosition; // �X�N���[�����W
    output.worldPosition = worldPosition.xyz; // ���[���h��ԍ��W
    output.normal = transformedNormal; // �@��
    output.texcoord = texcoord; // �e�N�X�`�����W
    output.intensity = intensity; // ���̋��x

    return output;
}
