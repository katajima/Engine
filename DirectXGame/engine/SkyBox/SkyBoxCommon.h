#pragma once
#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/Camera/Camera.h"




class DirectXCommon;
class SkyBoxCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCommonSetting();

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:// メンバ変数
	
	DirectXCommon* dxCommon_;

	std::unique_ptr<PSOManager> psoManager_;

	//ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;


};

