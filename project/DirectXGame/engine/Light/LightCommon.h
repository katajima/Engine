#pragma once
#include "DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"

#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

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

		// 初期化
		void Initialize(DirectXCommon* dxCommon);
		// 終了処理
		void Finalize();
		// ライン描画
		void DrawLight(IsLight is = { true,true,true }, int dire = 3, int point = 5, int spot = 6);
		// 更新
		void Update();
		// ライト追加
		void AddLight(std::shared_ptr<Lights> light) {
			light->SetNumber(int(m_lights.size()));
			m_lights.push_back(light);
			//count++;
		}
		// ライト数取得
		int GetLightSize() const { return static_cast<int>(m_lights.size()); }

		// ライト取得
		const std::vector<std::shared_ptr<Lights>>& GetLights() const {
			return m_lights;
		}

		// ライト開放
		void ClearLights() {
			for (auto& m_light : m_lights) {
				m_light.reset();
			}
			m_lights.clear();
		}

		// ライト開放(インデクスで)
		void ClearLight(int index) {
			m_lights[index].reset();
			m_lights.resize(m_lights.size());
		}

	private:
		// グローバルバリアブル適応
		void ApplyGlobalVariables();


	private:
		std::vector<std::shared_ptr<Lights>> m_lights;

		DirectXCommon* dxCommon = nullptr;

		const uint32_t kNumMaxInstance = 10;


		Microsoft::WRL::ComPtr < ID3D12Resource> directionalLightResource;
		DirectionalLightData* directionalLightData = nullptr;



		Microsoft::WRL::ComPtr < ID3D12Resource> pointLightResource;
		PointLightData* pointLightData;
		const int pointMax = 3;


		Microsoft::WRL::ComPtr < ID3D12Resource> spotLightResource;
		SpotLightData* spotLightData = nullptr;
		const int spotMax = 3;


		bool isLine = false;


		int count_ = 0;
	};
}

