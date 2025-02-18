#include "LightCommon.h"
#include "imgui.h"
#include "DirectXGame/engine/Line/LineCommon.h"

LightManager* LightManager::instance = nullptr;

LightManager* LightManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new LightManager;
	}
	return instance;
}

void LightManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	//平行光源用のリソースを作る
	directionalLightResource = dxCommon_->CreateBufferResource((sizeof(DirectionalLightData) * kNumMaxInstance));
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	
	// ポイントライト用のリソース
	pointLightResource = dxCommon_->CreateBufferResource((sizeof(PointLightData) * kNumMaxInstance));
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	//スポットライト用のリソースを作る
	spotLightResource = dxCommon_->CreateBufferResource(sizeof(SpotLightData) * kNumMaxInstance);
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
}

void LightManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void LightManager::DrawLight()
{
	////------平行光源用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	////------ポイントライト用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());

	////------スポットライト用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

}



void LightManager::Update()
{
    size_t pointLightIndex = 0;
    size_t directionalLightIndex = 0;
    size_t spotLightIndex = 0;

    // m_lightsからライトデータを取得して各バッファに格納
    for (const auto& light : m_lights)
    {
        
        if (light->GetType() == Lights::Type::Point)
        {
            static_cast<PointLight*>(light.get())->SetLightData(&pointLightData[pointLightIndex]);
            LineCommon::GetInstance()->AddLightLine(pointLightData[pointLightIndex]);
           
            ++pointLightIndex;
        }
        else if (light->GetType() == Lights::Type::Directional)
        {
            static_cast<DirectionalLight*>(light.get())->SetLightData(&directionalLightData[directionalLightIndex]);
            ++directionalLightIndex;
        }
        else if (light->GetType() == Lights::Type::Spot)
        {
            static_cast<SpotLight*>(light.get())->SetLightData(&spotLightData[spotLightIndex]);
            LineCommon::GetInstance()->AddLightLine(spotLightData[spotLightIndex]);
            ++spotLightIndex;
        }
    }

#ifdef _DEBUG
    ImGui::Begin("engine");

    if (ImGui::CollapsingHeader("Light Manager")) {
        // Directional Light UI
        if (ImGui::TreeNode("Directional Lights")) {
            for (size_t i = 0; i < directionalLightIndex; i++) {
                std::string label = "Directional " + std::to_string(i);
                if (ImGui::TreeNode(label.c_str())) {
                    bool is = static_cast<bool>(directionalLightData[i].isLight);
                    ImGui::Checkbox("Enable", &is);
                    directionalLightData[i].isLight = static_cast<int32_t>(is);
                    ImGui::DragFloat3("Direction", &directionalLightData[i].direction.x, 0.1f);
                    directionalLightData[i].direction = Normalize(directionalLightData[i].direction);
                    ImGui::DragFloat("Intensity", &directionalLightData[i].intensity, 0.1f, 0.0f, 10.0f);
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
                    bool is = static_cast<bool>(pointLightData[i].isLight);
                    ImGui::Checkbox("Enable", &is);
                    pointLightData[i].isLight = static_cast<int32_t>(is);
                    ImGui::DragFloat3("Position", &pointLightData[i].position.x, 0.1f);
                    ImGui::DragFloat("Intensity", &pointLightData[i].intensity, 0.1f, 0.0f, 10.0f);
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
                    bool is = static_cast<bool>(spotLightData[i].isLight);
                    ImGui::Checkbox("Enable", &is);
                    spotLightData[i].isLight = static_cast<int32_t>(is);
                    ImGui::DragFloat3("Position", &spotLightData[i].position.x, 0.1f);
                    ImGui::DragFloat3("Direction", &spotLightData[i].direction.x, 0.1f);
                    spotLightData[i].direction = Normalize(spotLightData[i].direction);
                    ImGui::DragFloat("Intensity", &spotLightData[i].intensity, 0.1f, 0.0f, 10.0f);
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
}

void LightManager::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int registr)
{
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	parameter.Descriptor.ShaderRegister = registr;
}

void LightManager::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	const char* gropName = "directionalLight";
	GlobalVariables::GetInstance()->CreateGroup(gropName);
	
	Vector3 di = globalVariables->GetVector3Value(gropName, "direction");
	directionalLightData->direction = di.Normalize();
	directionalLightData->intensity = globalVariables->GetFloatValue(gropName, "intensity");
	directionalLightData->color = globalVariables->GetVector4Value(gropName, "color");

	gropName = "pointlight";
	GlobalVariables::GetInstance()->CreateGroup(gropName);

	pointLightData[0].isLight   = globalVariables->GetBoolValue(gropName,    "p1 isLight");
	pointLightData[0].position  = globalVariables->GetVector3Value(gropName, "p1 position");
	pointLightData[0].decay     = globalVariables->GetFloatValue(gropName,   "p1 decay");
	pointLightData[0].radius    = globalVariables->GetFloatValue(gropName,   "p1 radius");
	pointLightData[0].intensity = globalVariables->GetFloatValue(gropName,   "p1 intensity");
	pointLightData[0].color     = globalVariables->GetVector4Value(gropName, "p1 color");
	pointLightData[1].isLight   = globalVariables->GetBoolValue(gropName,    "p2 isLight");
	pointLightData[1].position  = globalVariables->GetVector3Value(gropName, "p2 position");
	pointLightData[1].decay     = globalVariables->GetFloatValue(gropName,   "p2 deca");
	pointLightData[1].radius    = globalVariables->GetFloatValue(gropName,   "p2 radiu");
	pointLightData[1].intensity = globalVariables->GetFloatValue(gropName,   "p2 intensity");
	pointLightData[1].color     = globalVariables->GetVector4Value(gropName, "p2 color");

}





