#pragma once
#include "DirectXGame/engine/base/DirectXCommon.h"
#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include"DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/Camera/Camera.h"





class SkyBoxCommon
{
public:
	static SkyBoxCommon* instance;

	static SkyBoxCommon* GetInstance();


	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	void Update();

	void Finalize();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCommonSetting();

	void SetCamara(Camera* camara) { camara_ = camara; };

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:// メンバ変数
	Camera* camara_;
	
	DirectXCommon* dxCommon_;

	std::unique_ptr<PSOManager> psoManager_;

	//ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;

	// トランスフォームデータ
	std::unique_ptr<Transfomation> transfomation = nullptr;
	WorldTransform worldtransform_;
	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material;

};

