#pragma once
#include "DirectXGame/engine/base/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/PSO/PSOManager.h"




class Object3dCommon
{
public:
	
	enum class PSOType {

		UvInterpolation_MODE_SOLID_BACK,
		NoUvInterpolation_MODE_SOLID_BACK,
		UvInterpolation_MODE_WIREFRAME_BACK,
		NoUvInterpolation_MODE_WIREFRAME_BACK,
		
		UvInterpolation_MODE_SOLID_NONE,
		NoUvInterpolation_MODE_SOLID_NONE,
		UvInterpolation_MODE_WIREFRAME_NONE,
		NoUvInterpolation_MODE_WIREFRAME_NONE,
	};

	static Object3dCommon* instance;

	static Object3dCommon* GetInstance();


	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

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

	//ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature[2];
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[8];


	
	
};

