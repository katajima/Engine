// Ocean.hlsli
#define NUM_CONTROL_POINTS 4

struct VertexShaderOutput
{
    float4 position : SV_POSITION; // �X�N���[�����W
    float2 texcoord : TEXCOORD0; // �e�N�X�`�����W
    float3 normal : NORMAL; // �@��
    float3 worldPosition : WORLDPOS; // ���[���h���W
};

struct VS_INPUT
{
    float3 position : POSITION0; // ���_�ʒu
    float2 texcoord : TEXCOORD0; // �e�N�X�`�����W
    float3 normal : NORMAL0; // �@��
};

struct VS_CONTROL_POINT_OUTPUT
{
    float3 vPosition : WORLDPOS; // ���[���h���W�Ƃ��Ă̐���_�ʒu
};

struct PixelShaderInput
{
    float4 position : SV_POSITION; // �X�N���[�����W
    float2 texcoord : TEXCOORD0; // �e�N�X�`�����W
    float3 normal : NORMAL; // �@��
    float3 worldPosition : WORLDPOS; // ���[���h���W
};
////------PixelShader------////
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};




// �h���C���V�F�[�_�[�̏o�͍\����
struct DS_OUTPUT
{
    float4 vPosition : SV_POSITION; // �X�N���[�����W
    float2 texcoord : TEXCOORD0; // �e�N�X�`�����W
    float3 vNormal : NORMAL; // �@��
    float3 vWorldPos : WORLDPOS; // ���[���h���W
};

// �n���V�F�[�_�[����n�����f�[�^�\��
struct HS_CONTROL_POINT_OUTPUT
{
    float3 vPosition : WORLDPOS; // ���[���h���W
};

// �n���V�F�[�_�[�̒萔�f�[�^
struct HS_CONSTANT_DATA_OUTPUT
{
    // 4�ӂ̃e�b�Z���[�V�����W�����i�[����z��i4 �Ӂj
    float EdgeTessFactor[4] : SV_TessFactor;

    // �����̃e�b�Z���[�V�����W�����i�[����z��i2 �l�j
    float InsideTessFactor[2] : SV_InsideTessFactor;
};


struct TransformationMatrix
{
    float4x4 WVP; // ���[���h�E�r���[�E�v���W�F�N�V�����s��
    float4x4 World; // ���[���h�s��
    float4x4 WorldInverseTranspose; // ���[���h�s��̋t�]�u�s��i�@���v�Z�p�j
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);



