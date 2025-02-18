#include "Line.hlsli"

//StructuredBuffer<LineGPU> gLine : register(t0);

struct Camera
{
    float4x4 worldViewProjMatrix; // ���[���h�E�r���[�E�v���W�F�N�V�����s��
};
ConstantBuffer<Camera> gCamera : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float4 color : COLOR;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

  
    // ���[���h�E�r���[�E�v���W�F�N�V�����s���K�p
    output.position = mul(input.position, gCamera.worldViewProjMatrix);
    output.color = input.color; // ���̂܂܃s�N�Z���V�F�[�_�[�֓n��
    
    return output;
}
