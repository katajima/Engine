#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/base/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/PSO/PSOManager.h"

class PrimitiveCommon
{
public:
	static PrimitiveCommon* instance;
	static PrimitiveCommon* GetInstance();


	void Initialize(DirectXCommon* dxcommon);

	void Finalize();

	void DrawCommonSetting();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }
private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:
	DirectXCommon* dxCommon_;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	//ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
};

