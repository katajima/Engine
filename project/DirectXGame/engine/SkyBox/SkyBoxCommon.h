#pragma once

#include"DirectXGame/engine/PSO/PSOManager.h"



// 前方宣言
class DirectXCommon;

/// <summary>
/// スカイボックス共通クラス
/// </summary>
class SkyBoxCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// DirectX共通クラス取得
	DirectXCommon* GetDxCommon() const { return dxCommon_; }
	// 描画前準備
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

