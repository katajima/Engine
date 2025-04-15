#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

// 3Dモデル共通部分
class DirectXCommon;
class CameraCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister);
	static void SetRootParameterVertex(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister);

private:
	DirectXCommon* dxCommon_;

};