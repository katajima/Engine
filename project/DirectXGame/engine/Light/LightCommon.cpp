#include "LightCommon.h"
#include "imgui.h"
#include "DirectXGame/engine/Line/LineCommon.h"



Engine::LightManager::~LightManager()
{
	// EntityManager破棄時にライト用GPUリソースを確実に解放する
	Finalize();
}

void Engine::LightManager::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;	// DX共通クラス

	//平行光源用のリソースを作る
	directionalLightResource = dxCommon->GetDXGIDevice()->CreateBufferResource((sizeof(DirectionalLightData) * kNumMaxInstance));
	directionalLightResource->SetName(L"Light Directional");
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// ポイントライト用のリソース
	pointLightResource = dxCommon->GetDXGIDevice()->CreateBufferResource((sizeof(PointLightData) * kNumMaxInstance));
	pointLightResource->SetName(L"Light Point");
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	//スポットライト用のリソースを作る
	spotLightResource = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(SpotLightData) * kNumMaxInstance);
	spotLightResource->SetName(L"Light Spot");
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
}

void Engine::LightManager::Finalize()
{
	// ライト一覧を先に解放して、GPU転送先ポインタへの参照を残さないようにする
	ClearLights();

	// 平行光源用Uploadリソースを閉じる
	if (directionalLightResource && directionalLightData) {
		directionalLightResource->Unmap(0, nullptr);
		directionalLightData = nullptr;
	}
	// ポイントライト用Uploadリソースを閉じる
	if (pointLightResource && pointLightData) {
		pointLightResource->Unmap(0, nullptr);
		pointLightData = nullptr;
	}
	// スポットライト用Uploadリソースを閉じる
	if (spotLightResource && spotLightData) {
		spotLightResource->Unmap(0, nullptr);
		spotLightData = nullptr;
	}

	directionalLightResource.Reset();
	pointLightResource.Reset();
	spotLightResource.Reset();
	dxCommon = nullptr;
}


void Engine::LightManager::DrawLight(IsLight is, int dire, int point, int spot)
{
	////------平行光源用------////
	if (is.dire) {
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(dire, directionalLightResource->GetGPUVirtualAddress());
	}
	////------ポイントライト用------////
	if (is.pount) {
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(point, pointLightResource->GetGPUVirtualAddress());
	}
	////------スポットライト用------////
	if (is.spot) {
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(spot, spotLightResource->GetGPUVirtualAddress());
	}
}



void Engine::LightManager::Update()
{
	ZeroMemory(pointLightData, sizeof(PointLightData) * kNumMaxInstance);
	ZeroMemory(directionalLightData, sizeof(DirectionalLightData) * kNumMaxInstance);
	ZeroMemory(spotLightData, sizeof(SpotLightData) * kNumMaxInstance);

	size_t pointLightIndex = 0;
	size_t directionalLightIndex = 0;
	size_t spotLightIndex = 0;

	// m_lights から GPU に送るバッファを更新
	for (const auto& light : m_lights)
	{
		if (!light) continue;

		if (light->GetType() == Lights::Type::Point)
		{
			light->SetLightData(&pointLightData[pointLightIndex]);
			// LineCommon::GetInstance()->AddLightLine(pointLightData[pointLightIndex]);
			++pointLightIndex;
		}
		else if (light->GetType() == Lights::Type::Directional)
		{
			light->SetLightData(&directionalLightData[directionalLightIndex]);
			++directionalLightIndex;
		}
		else if (light->GetType() == Lights::Type::Spot)
		{
			light->SetLightData(&spotLightData[spotLightIndex]);
			//  LineCommon::GetInstance()->AddLightLine(spotLightData[spotLightIndex]);
			++spotLightIndex;
		}
	}

#ifdef _DEBUG
	ImGui::Begin("Light Properties");

	if (ImGui::CollapsingHeader("Light Manager")) {
		// Directional Light UI
		if (ImGui::TreeNode("Directional Lights")) {
			for (size_t i = 0; i < directionalLightIndex; i++) {
				std::string label = "Directional " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str())) {
					ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&directionalLightData[i].isLight));
					ImGui::DragFloat3("Direction", &directionalLightData[i].direction.x, 0.1f);
					ImGui::DragFloat("Intensity", &directionalLightData[i].intensity, 0.1f, 0.0f, 100.0f);
					ImGui::ColorEdit3("Color", &directionalLightData[i].color.x);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		// Point Light UI
		if (ImGui::TreeNode("Point Lights")) {
			for (size_t i = 0; i < pointLightIndex; i++) {
				std::string label = "Point " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str())) {
					ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&pointLightData[i].isLight));
					ImGui::DragFloat3("Position", &pointLightData[i].position.x, 0.1f);
					ImGui::DragFloat("Intensity", &pointLightData[i].intensity, 0.1f, 0.0f, 100.0f);
					ImGui::DragFloat("Decay", &pointLightData[i].decay, 0.1f);
					ImGui::DragFloat("Radius", &pointLightData[i].radius, 0.1f);
					ImGui::ColorEdit3("Color", &pointLightData[i].color.x);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		// Spot Light UI
		if (ImGui::TreeNode("Spot Lights")) {
			for (size_t i = 0; i < spotLightIndex; i++) {
				std::string label = "Spot " + std::to_string(i);
				if (ImGui::TreeNode(label.c_str())) {
					ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&spotLightData[i].isLight));
					ImGui::DragFloat3("Position", &spotLightData[i].position.x, 0.1f);
					ImGui::DragFloat3("Direction", &spotLightData[i].direction.x, 0.1f);
					spotLightData[i].direction = Normalize(spotLightData[i].direction);
					ImGui::DragFloat("Intensity", &spotLightData[i].intensity, 0.1f, 0.0f, 100.0f);
					ImGui::DragFloat("Distance", &spotLightData[i].distance, 0.1f);
					ImGui::DragFloat("Decay", &spotLightData[i].decay, 0.1f);
					ImGui::DragFloat("Falloff Start", &spotLightData[i].cosFalloffStart, 0.1f);
					ImGui::DragFloat("Cos Angle", &spotLightData[i].cosAngle, 0.01f);
					ImGui::ColorEdit3("Color", &spotLightData[i].color.x);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}

	ImGui::End();
#endif
	for (size_t i = 0; i < directionalLightIndex; i++) {
		directionalLightData[i].direction = Normalize(directionalLightData[i].direction);
	}

	// **ImGuiの変更を各ライトクラスに反映**
	size_t pIndex = 0;
	size_t dIndex = 0;
	size_t sIndex = 0;

	for (auto& light : m_lights)
	{
		if (!light) continue;

		if (light->GetType() == Lights::Type::Point)
			light->UpdateFromData(&pointLightData[pIndex++]);
		else if (light->GetType() == Lights::Type::Directional)
			light->UpdateFromData(&directionalLightData[dIndex++]);
		else if (light->GetType() == Lights::Type::Spot)
			light->UpdateFromData(&spotLightData[sIndex++]);
	}

}



void Engine::LightManager::ApplyGlobalVariables()
{

}





