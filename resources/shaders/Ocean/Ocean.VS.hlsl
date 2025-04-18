#include"Ocean.hlsli"





VS_CONTROL_POINT_OUTPUT main(VS_INPUT input)
{
    VS_CONTROL_POINT_OUTPUT output;

    // ワールド座標系に変換
    float4 worldPosition = mul(float4(input.position, 1.0), gTransformationMatrix.World);
    output.vPosition = worldPosition.xyz;

    return output;
}
