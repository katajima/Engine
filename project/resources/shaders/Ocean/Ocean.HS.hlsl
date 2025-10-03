
#include "Ocean.hlsli"



HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
    InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
    uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

  // DirectXの上限値（通常64）を考慮
    const float MAX_TESS_FACTOR = 64.0f;

    // 各辺のテッセレーション係数
    Output.EdgeTessFactor[0] = MAX_TESS_FACTOR; // 辺0
    Output.EdgeTessFactor[1] = MAX_TESS_FACTOR; // 辺1
    Output.EdgeTessFactor[2] = MAX_TESS_FACTOR; // 辺2
    Output.EdgeTessFactor[3] = MAX_TESS_FACTOR; // 辺3

    // 内部テッセレーション係数
    Output.InsideTessFactor[0] = MAX_TESS_FACTOR; // 内部1
    Output.InsideTessFactor[1] = MAX_TESS_FACTOR; // 内部2

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
