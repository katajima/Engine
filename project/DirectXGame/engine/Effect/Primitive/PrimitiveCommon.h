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
			kDefault,
			kRingClamp,
			kNoCull,
			kNoCullRingClamp,
			kNoCullWireFrame,
		};
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxcommon);
		/// <summary>
		/// 描画準備
		/// </summary>
		void DrawCommonSetting(PsoType type = PsoType::kDefault);
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon; }
	private:
		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		void CreateRootSignature();
		/// <summary>
		/// グラフィックスパイプラインの作成
		/// </summary>
		void CreateGraphicsPipeline();

	private:
		DirectXCommon* dxCommon = nullptr;

		std::unique_ptr<PSOManager> psoManager_ = nullptr;

		PSRS default_;           // 標準プリミティブ用PSO
		PSRS defaultRing_;       // リング用PSO
		PSRS noCull_;           // カリングなし用PSO
		PSRS noCullRing_;       // カリングなしリング用PSO
		PSRS noCullWireFrame_;  // カリングなしワイヤーフレーム用PSO

	public:



	};
}
