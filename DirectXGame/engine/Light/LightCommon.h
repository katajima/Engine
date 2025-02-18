#pragma once
#include "DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/3d/Object3dCommon.h"
#include"DirectXGame/engine/Line/Line.h"
#include"DirectXGame/engine/Line/LineCommon.h"

#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

#include"Lights.h"

class LightManager
{
public:
	static LightManager* instance;

	static LightManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	void DrawLight();

	void DrawLightLine();

	void SetLineCamera(Camera* camera);

	void Update();

	void AddLight(std::shared_ptr<Lights> light) {
		m_lights.push_back(light);
	}

	const std::vector<std::shared_ptr<Lights>>& GetLights() const {
		return m_lights;
	}
	
	static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int registr);

private:
	void ApplyGlobalVariables();

	
private:
	std::vector<std::shared_ptr<Lights>> m_lights;

	DirectXCommon* dxCommon_ = nullptr;

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

};


