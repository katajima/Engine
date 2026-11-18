#pragma once
#include "DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include <algorithm>
#include<format>
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"

#include"DirectXGame/engine/GlobalVariables/GlobalVariables.h"

#include"Lights.h"

namespace Engine {
	/// <summary>
	/// ライトマネージャークラス
	/// </summary>
	class LightManager
	{
	public:
		~LightManager();
		// ライト使うかの構造体
		struct IsLight {
			bool dire;
			bool pount;
			bool spot;
		};

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// ライン描画
		/// </summary>
		void DrawLight(IsLight is = { true,true,true }, int dire = 3, int point = 5, int spot = 6);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// @brief GPU転送領域の上限を超えない場合にライトを登録する。
		/// @param light 登録するライト。nullptrは登録しない。
		/// @return 登録できた場合はtrue。同じ種類が10個ある場合はfalse。
		bool AddLight(std::shared_ptr<Lights> light);
		/// <summary>
		/// ライト数取得
		/// </summary>
		int GetLightSize() const { return static_cast<int>(m_lights.size()); }

		/// <summary>
		/// ライト取得
		/// </summary>
		const std::vector<std::shared_ptr<Lights>>& GetLights() const {
			return m_lights;
		}

		/// <summary>
		/// ライト開放
		/// </summary>
		void ClearLights() {
			for (auto& m_light : m_lights) {
				m_light.reset();
			}
			m_lights.clear();
		}

		/// <summary>
		/// ライト開放(インデクスで)
		/// </summary>
		void ClearLight(int index) {
			m_lights[index].reset();
			m_lights.resize(m_lights.size());
		}

	private:
		/// <summary>
		/// グローバルバリアブル適応
		/// </summary>
		void ApplyGlobalVariables();


	private:
		std::vector<std::shared_ptr<Lights>> m_lights;

		DirectXCommon* dxCommon = nullptr;

		static constexpr uint32_t kNumMaxInstance = 10;


		Microsoft::WRL::ComPtr < ID3D12Resource> directionalLightResource;
		DirectionalLightData* directionalLightData = nullptr;



		Microsoft::WRL::ComPtr < ID3D12Resource> pointLightResource;
		PointLightData* pointLightData = nullptr;
		const int pointMax = 3;


		Microsoft::WRL::ComPtr < ID3D12Resource> spotLightResource;
		SpotLightData* spotLightData = nullptr;
		const int spotMax = 3;


		bool isLine = false;


		int count_ = 0;
	};
}

