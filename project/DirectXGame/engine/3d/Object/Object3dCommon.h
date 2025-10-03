#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/PSO/PSOManager.h"




class Object3dCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCommonSetting(PSOType type){ psoManager_->DrawSetting(type, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);}

	void SetDefaltCamera(Camera* camera) { this->defaultCamera = camera; }

	Camera* GetDefaltCamera() const { return defaultCamera; }

	int32_t count = 0;
private:
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();
private:// メンバ変数
	DirectXCommon* dxCommon_;

	Camera* defaultCamera = nullptr;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;



private:// メンバ変数


};

