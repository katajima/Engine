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
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/PSO/PSOManager.h"


namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// プリミティブ共通クラス
	/// </summary>
	class PrimitiveCommon
	{
	public:

		enum class PsoType {
			kDefalt,
			kRingClamp,
			kNoCull,
			kNoCullRingClamp,
			kNoCullWireFrame,
		};
		// 初期化
		void Initialize(DirectXCommon* dxcommon);
		// 描画準備
		void DrawCommonSetting(PsoType type = PsoType::kDefalt);
		// DirectX共通クラス取得
		DirectXCommon* GetDxCommon() const { return dxCommon; }
	private:
		// ルートシグネチャの作成
		void CreateRootSignature();
		// グラフィックスパイプラインの作成
		void CreateGraphicsPipeline();

	private:
		DirectXCommon* dxCommon = nullptr;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		PSRS defalt_;           // 標準プリミティブ用PSO
		PSRS defaltRing_;       // リング用PSO
		PSRS noCull_;           // カリングなし用PSO
		PSRS noCullRing_;       // カリングなしリング用PSO
		PSRS noCullWireFrame_;  // カリングなしワイヤーフレーム用PSO

	public:



	};
}
