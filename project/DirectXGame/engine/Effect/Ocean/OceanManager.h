#pragma once
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include"DirectXGame/engine/PSO/PSOManager.h"

#include<random>
#include<numbers>

namespace Engine {
	/// <summary>
	/// 海マネージャー
	/// </summary>
	class OceanManager
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// 描画設定
		/// </summary>
		void DrawCommonSetting();

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
		// ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		// パイプラインステートオブジェクト
		Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;
		//
		std::unique_ptr<PSOManager> psoManager_ = nullptr;
	private:
		DirectXCommon* dxCommon = nullptr;
	};
}

