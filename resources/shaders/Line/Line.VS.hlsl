#include "Line.hlsli"

//StructuredBuffer<LineGPU> gLine : register(t0);

struct Camera
{
    float4x4 worldViewProjMatrix; // ワールド・ビュー・プロジェクション行列
};
ConstantBuffer<Camera> gCamera : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

  
    // ワールド・ビュー・プロジェクション行列を適用
    output.position = mul(input.position, gCamera.worldViewProjMatrix);

    return output;
}
