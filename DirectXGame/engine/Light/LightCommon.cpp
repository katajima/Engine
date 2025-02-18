#include "LightCommon.h"
#include "imgui.h"

LightCommon* LightCommon::instance = nullptr;

LightCommon* LightCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new LightCommon;
	}
	return instance;
}

void LightCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	//平行光源用のリソースを作る
	directionalLightResource = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	
	//今回は赤を書き込んで見る //白
	directionalLightData[0] = DirectionalLight({1.0f,1.0f,1.0f,1.0f}, {0.0f,-1.0f,0.0f}, 1.0f);
	directionalLightData[0].lig = 0.2f;
	directionalLightData[0].isLight = true;
						
	// 半球ライト		 
	directionalLightData[0].groundColor = { 0.7f,0.5f,0.3f };
	directionalLightData[0].skyColor = { 0.15f,0.7f,0.95f };
	directionalLightData[0].groundColor = { 1.0f,1.0f,1.0f };
	directionalLightData[0].skyColor = { 1.0f,1.0f,1.0f };
	directionalLightData[0].groundNormal = { 0.0f,1.0f,0.0f };


	pointLightResource = dxCommon_->CreateBufferResource((sizeof(PointLight) * kNumMaxInstance));
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));


	


	////今回は赤を書き込んで見る //白
	//pointLightData[0] = PointLight({1.0f,1.0f,1.0f,1.0f}, {0.0f,-1.0f,-10.0f});
	//pointLightData[0].radius = 10.0f;
	//pointLightData[0].intensity = 10.0f;
	//pointLightData[0].lig = 0.2f;
	//pointLightData[0].isLight = true;
	//pointLightData[1] = PointLight({0.0f,1.0f,1.0f,1.0f}, {0.0f,-1.0f,10.0f});
	//pointLightData[1].radius = 10.0f;
	//pointLightData[1].intensity = 5.0f;
	//pointLightData[1].isLight = true;
	//pointLightData[1].lig = 0.2f;




	//平行光源用のリソースを作る
	spotLightResource = dxCommon_->CreateBufferResource(sizeof(SpotLight) * kNumMaxInstance);
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	//今回は赤を書き込んで見る //白

	for (int i = 0; i < spotMax; i++) {
		spotLightData[i] = SpotLight({1.0f,1.0f,1.0f,1.0f}, {2.0f,1.25f,0.0f}, 10.0f, Normalize({-1.0f,-1.0f,0.0f}), 10.0f, 2.0f, std::cos(std::numbers::pi_v<float> / 3.0f), 1.0f);
		spotLightData[i].position = Vector3(10 * float(i), 0, 10);
		spotLightData[i].intensity = 50.0f;
		spotLightData[i].isLight = true;
	}

	
}

void LightCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void LightCommon::DrawLight()
{
	////------平行光源用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	////------ポイントライト用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());

	////------スポットライト用------////
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

}

void LightCommon::SetLineCamera(Camera* camera)
{
	
}

void LightCommon::Update()
{
	
#ifdef _DEBUG
	ImGui::Begin("engine");

	if (ImGui::CollapsingHeader("Light")) {

		//if (ImGui::CollapsingHeader("DirectionalLight")) {
			if (ImGui::BeginTabBar("directionalLightData"))
			{
				if (ImGui::BeginTabItem("directionalLightData"))
				{
					bool is = directionalLightData->isLight;
					ImGui::Checkbox("isLighting", &is);
					directionalLightData->isLight = is;
					ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.1f);
					directionalLightData->direction = Normalize(directionalLightData->direction);
					ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.1f);
					if (0 >= directionalLightData->intensity)
						directionalLightData->intensity = 0;
					ImGui::DragFloat("lig", &directionalLightData->lig, 0.1f);
					ImGui::ColorEdit3("groundColor", &directionalLightData->groundColor.x);
					ImGui::ColorEdit3("skyColor", &directionalLightData->skyColor.x);

					ImGui::ColorEdit4("color", &directionalLightData->color.x);
					ImGui::EndTabItem();

					if (ImGui::Button("save")) {
						
						GlobalVariables* globalVariables = GlobalVariables::GetInstance();
						const char* gropName = "directionalLight";
						globalVariables->saveFile(gropName);
					}
				}
			}
		//}
		ImGui::EndTabBar();
		if (ImGui::BeginTabBar("pointLightData"))
		{
			for (int i = 0; i < 2; i++) {
				std::string str = "light" + std::to_string(i);
				if (ImGui::BeginTabItem(str.c_str())) {
					bool is = pointLightData[i].isLight;
					ImGui::Checkbox("isLighting", &is);
					pointLightData[i].isLight = is;
					ImGui::DragFloat3("position", &pointLightData[i].position.x, 0.1f);
					ImGui::DragFloat("intensity", &pointLightData[i].intensity, 0.1f);
					if (0 >= pointLightData[i].intensity)
						pointLightData[i].intensity = 0;
					ImGui::DragFloat("decay", &pointLightData[i].decay, 0.1f);
					ImGui::DragFloat("radius", &pointLightData[i].radius, 0.1f);
					ImGui::DragFloat("lig", &pointLightData[i].lig, 0.1f);

					ImGui::ColorEdit4("color", &pointLightData[i].color.x);
					ImGui::EndTabItem();
				}
			}
		}
		ImGui::EndTabBar();
		if (ImGui::BeginTabBar("spotLightData"))
		{
			for (int i = 0; i < spotMax; i++) {
				std::string str = "light" + std::to_string(i);
				if (ImGui::BeginTabItem(str.c_str())) {
					bool is = spotLightData[i].isLight;
					ImGui::Checkbox("isLighting", &is);
					spotLightData[i].isLight = is;

					ImGui::DragFloat3("position", &spotLightData[i].position.x, 0.1f);
					
					ImGui::DragFloat3("direction", &spotLightData[i].direction.x, 0.1f);
					spotLightData[i].direction = Normalize(spotLightData[i].direction);
					
					ImGui::DragFloat("intensity", &spotLightData[i].intensity, 0.1f);
					if (0 >= spotLightData[i].intensity)
						spotLightData[i].intensity = 0;
					ImGui::DragFloat("distance", &spotLightData[i].distance, 0.1f);
					
					ImGui::DragFloat("decay", &spotLightData[i].decay, 0.1f);
					ImGui::DragFloat("cosFalloffStart", &spotLightData[i].cosFalloffStart, 0.1f);
					ImGui::DragFloat("cosAngle", &spotLightData[i].cosAngle, 0.01f);

					ImGui::ColorEdit4("color", &spotLightData[i].color.x);
					ImGui::EndTabItem();
				}
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	
#endif


	

}

void LightCommon::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int registr)
{
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	parameter.Descriptor.ShaderRegister = registr;
}

void LightCommon::ApplyGlobalVariables()
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


void LightCommon::DrawLightLine() {
	
}


