#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/PSO/PSOManager.h"




class Object3dCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCommonSetting(PSOType type);

	void AllDraw();

	void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }

	Camera* GetDefaltCamera() const { return defaultCamera; }

	int32_t count = 0;
private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();
private:// メンバ変数
	DirectXCommon* dxCommon_;

	Camera* defaultCamera = nullptr;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature[2];
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[8];

	// 透過用

	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignatureTransparent;
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineStateTransparent;

private:// メンバ変数


};

