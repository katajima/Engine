#include "Thunder.hlsli"

// �萔�o�b�t�@�ŊO���p�����[�^���擾
struct ThunderParameters
{
    float falloffRate; // ������
    float3 baseColorStart; // ���̈Â������̐F
    float3 baseColorEnd; // ���̖��邢�����̐F
    float alphaScale; // �A���t�@�l�X�P�[��
};
ConstantBuffer<ThunderParameters> gThunderParameters : register(b1);

float4 PixelShaderFunction(ShaderIO input) : SV_Target
{
    // ���̋P���v�Z
    float glow = exp(-pow(input.intensity *gThunderParameters.falloffRate, 2.0)); // ���S�Ŗ��邭�O���Ō���

    // ���̐F (�O���p�����[�^�Őݒ�)
    float3 baseColor = lerp(gThunderParameters.baseColorStart, gThunderParameters.baseColorEnd, glow);

    // �t�F�[�h���� (�A���t�@�l�𒲐��\��)
    float alpha = glow * gThunderParameters.alphaScale;

    return float4(baseColor * glow, alpha);
}
