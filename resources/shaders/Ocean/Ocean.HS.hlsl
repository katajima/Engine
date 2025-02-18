
#include "Ocean.hlsli"



HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
    InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

  // DirectX�̏���l�i�ʏ�64�j���l��
    const float MAX_TESS_FACTOR = 64.0f;

    // �e�ӂ̃e�b�Z���[�V�����W��
    Output.EdgeTessFactor[0] = MAX_TESS_FACTOR; // ��0
    Output.EdgeTessFactor[1] = MAX_TESS_FACTOR; // ��1
    Output.EdgeTessFactor[2] = MAX_TESS_FACTOR; // ��2
    Output.EdgeTessFactor[3] = MAX_TESS_FACTOR; // ��3

    // �����e�b�Z���[�V�����W��
    Output.InsideTessFactor[0] = MAX_TESS_FACTOR; // ����1
    Output.InsideTessFactor[1] = MAX_TESS_FACTOR; // ����2

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
