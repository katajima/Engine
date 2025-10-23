#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/PSO/CSPSOManager.h"


/// <summary>
/// スキニング共通クラス
/// </summary>
class SkinningConmmon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// DirectX共通クラス取得
	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	// 描画前準備
	void DrawCommonSetting(PSOType type){psoManager_->DrawSetting(type);}

	// 計算前準備
	void DrawComputeSetting(){csPsoManager_->PreComputePSRS();}
	// デフォルトカメラ設定
	void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }
	// デフォルトカメラ取得
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

