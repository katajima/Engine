
#include "Ocean.hlsli"



HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
    InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    // �e�ӂ̃e�b�Z���[�V�����W����ݒ�i�����ł͗�Ƃ���15��ݒ�j
    Output.EdgeTessFactor[0] = 1000000.0f; // ��0
    Output.EdgeTessFactor[1] = 1000000.0f; // ��1
    Output.EdgeTessFactor[2] = 1000000.0f; // ��2
    Output.EdgeTessFactor[3] = 1000000.0f; // ��3

    // �����e�b�Z���[�V�����W����ݒ�i�����ł͗�Ƃ���15��ݒ�j
    Output.InsideTessFactor[0] = 1000000.0f; // �����e�b�Z���[�V�����W��1
    Output.InsideTessFactor[1] = 1000000.0f; // �����e�b�Z���[�V�����W��2

    return Output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	Output.vPosition = ip[i].vPosition;

	return Output;
}
