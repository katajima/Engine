#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/PSO/PSOManager.h"

class DirectXCommon;
class PrimitiveCommon
{
public:
	enum class PsoType {
		kDefalt,
		kRingClamp,
	};

	void Initialize(DirectXCommon* dxcommon);

	void DrawCommonSetting(PsoType type = PsoType::kDefalt);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }
private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

private:
	DirectXCommon* dxCommon_;

	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	struct PrimitivePSO
	{
		//ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
	};
	PrimitivePSO defalt_;
	PrimitivePSO defaltRing_;
public:



};

