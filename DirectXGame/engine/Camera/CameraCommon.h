#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

// 3Dモデル共通部分
class Input;
class DirectXCommon;
class CameraCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void SetInput(Input* input) { input_ = input; };

	static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister);
	static void SetRootParameterVertex(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister);

	Input* GetInput() { return input_;};

private:
	DirectXCommon* dxCommon_;

	Input* input_;
};