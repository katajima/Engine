// Thunder.hlsli

// ���_�V�F�[�_�[����уs�N�Z���V�F�[�_�[�Ԃ̃f�[�^�\��
struct ShaderIO
{
    float4 position : SV_POSITION; // �X�N���[�����W
    float2 texcoord : TEXCOORD0; // �e�N�X�`�����W (UV)
    float3 normal : NORMAL0; // �@���x�N�g�� (���[���h���)
    float3 worldPosition : POSITION0; // ���[���h���W
    float intensity : TEXCOORD1; // ���̋P�����x
};


struct TransformationMatrix
{
    float4x4 WVP; // ���[���h�E�r���[�E�v���W�F�N�V�����s��
    float4x4 World; // ���[���h�s��
    float4x4 WorldInverseTranspose; // ���[���h�s��̋t�]�u�s��i�@���v�Z�p�j
    float time; // ���� (�b�P��)
    float3 padding; // �A���C�����g�����p
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);