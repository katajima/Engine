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
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/Transform/Transformation/Transformation.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"

#include<random>
#include<numbers>


// 前方宣言
namespace Engine {

	class OceanManager;
	class EntityManager;
	class DirectXCommon;

	/// <summary>
	/// 海クラス
	/// </summary>
	class Ocean
	{
	private:
		// ウェーブデータ
		struct WaveParameters {
			Vector2 waveDirection; // 波の方向
			float amplitude; // 波の振幅
			float frequency; // 波の周波数
			float speed; // 波の速度
			float time; // 現在の時間（アニメーション用）
			int flag;   // 波が起きるのか
			float pad[1];
		};
	public:


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EntityManager* entityManager, Vector2 range);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// ImGui更新
		/// </summary>
		void UpdateImgui();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// マテリアル取得
		/// </summary>
		Material* GetMaterial() { return material.get(); }
		/// <summary>
		/// モデルメッシュ取得
		/// </summary>
		ModelMesh* GetMesh() { return mesh_.get(); }

		/// <summary>
		/// 波パラメーターデータ取得
		/// </summary>
		WaveParameters* GetWaveParameters() const { return cbWaveResource_.Data(); }

	private:

		/// <summary>
		/// ウェーブ追加
		/// </summary>
		void AddWave();

	private:
		// マテリアル
		std::unique_ptr<Material> material = nullptr;
		// メッシュ
		std::unique_ptr<ModelMesh> mesh_ = nullptr;
	private:


		ConstantBuffer<WaveParameters> cbWaveResource_;

		// ノイズデータ
		struct NoiseData {
			float noiseScale; // ノイズのスケール
			float noiseStrength; // ノイズの強度
			int octaves; // フラクタルノイズのオクターブ数
			float roughness; // 各オクターブの影響度
		};

		ConstantBuffer<NoiseData> cbNoiseResource_;


		// 幅	
		struct Range {
			Vector2 range;
		};
		Range range_;

		// 波の合成数
		uint32_t index_ = 0;

	private:
		EntityManager* entityManager = nullptr;
		DirectXCommon* dxCommon = nullptr;
	};
}