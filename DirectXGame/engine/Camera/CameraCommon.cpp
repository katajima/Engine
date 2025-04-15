#include "CameraCommon.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void CameraCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
}

void CameraCommon::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister)
{
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	parameter.Descriptor.ShaderRegister = ShaderRegister;
}

void CameraCommon::SetRootParameterVertex(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister)
{
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	parameter.Descriptor.ShaderRegister = ShaderRegister;
}


