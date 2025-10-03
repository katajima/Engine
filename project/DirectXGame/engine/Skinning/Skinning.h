#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/PSO/CSPSOManager.h"


class SkinningConmmon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	
	void DrawCommonSetting(PSOType type){psoManager_->DrawSetting(type);}

	void DrawComputeSetting(){csPsoManager_->PreComputePSRS();}

	void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }

	Camera* GetDefaltCamera() const {
		defaultCamera;
		return defaultCamera;
	}

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


private:// メンバ変数
	DirectXCommon* dxCommon_;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	std::unique_ptr<CSPSOManager> csPsoManager_ = nullptr;


	Camera* defaultCamera = nullptr;
};

