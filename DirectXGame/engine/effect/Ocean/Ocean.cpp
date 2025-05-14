#include "Ocean.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "OceanManager.h"
#include "imgui.h"

void Ocean::Initialize(Entity3DManager* entity3dManager,Vector2 renge)
{
	entity3dManager_ = entity3dManager;
	directXCommon_ = entity3dManager_->GetOceanManager()->GetDxCommon();


	renge_.renge = renge;

	mesh_ = std::make_unique<Mesh>();
	// 頂点データ
	mesh_->vertices.push_back({ .position = {renge_.renge.x, 0.0f, renge_.renge.y, 1.0f} ,.texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左上
	mesh_->vertices.push_back({ .position = {-renge_.renge.x, 0.0f, renge_.renge.y, 1.0f} ,.texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右上
	mesh_->vertices.push_back({ .position = {renge_.renge.x, 0.0f, -renge_.renge.y, 1.0f} ,.texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左下
	mesh_->vertices.push_back({ .position = {-renge_.renge.x, 0.0f, -renge_.renge.y, 1.0f} ,.texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右下
	
	mesh_->indices.push_back(0);
	mesh_->indices.push_back(1);
	mesh_->indices.push_back(2);

	mesh_->indices.push_back(1);
	mesh_->indices.push_back(3);
	mesh_->indices.push_back(2);

	mesh_->Initialize(directXCommon_);





	
	material = std::make_unique<Material>();
	material->Initialize(entity3dManager_->GetOceanManager()->GetDxCommon());
	material->tex_.diffuseFilePath = "resources/Texture/Image.png";
	material->tex_.environmentFilePath = "resources/Texture/hdr/sky.dds";
	material->LoadTex();
	material->color = { 0,0,1,1.0f };


	noiseResource = directXCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(NoiseData));
	noiseResource->Map(0, nullptr, reinterpret_cast<void**>(&noiseData));

	noiseData->noiseScale = 10.0f;
	noiseData->noiseStrength = 1.0f;
	noiseData->octaves = 37;
	noiseData->roughness = 10.0f;



	waveResource = directXCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(WaveParameters) * 10);
	waveResource->Map(0, nullptr, reinterpret_cast<void**>(&waveData));

	waveData[0].amplitude = 1.500f;
	waveData[0].frequency = 3.340f;
	waveData[0].speed = 1.0f;
	waveData[0].time = 0;
	waveData[0].waveDirection = { 0,1 };
	waveData[0].flag = true;
	
	waveData[1].amplitude = 1.0f;
	waveData[1].frequency = 2.0f;
	waveData[1].speed = 1.0f;
	waveData[1].time = 0;
	waveData[1].waveDirection = { 0.5f,0.5f };
	waveData[1].flag = true;
	
	waveData[2].amplitude = 0.5f;
	waveData[2].frequency = 2.0f;
	waveData[2].speed = 1.0f;
	waveData[2].time = 0;
	waveData[2].waveDirection = { 0,1 };
	waveData[2].flag = true;

	index_ = 3;
}

void Ocean::Update()
{
	for (size_t i = 0; i < index_; ++i) {
		waveData[i].time += 1.0f / 60.0f;
	}
	material->GPUData();
}

void Ocean::UpdateImgui()
{
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Wave")) {
		ImGui::Text("noiseData");
		ImGui::Separator();
		ImGui::DragFloat("noiseScale", &noiseData->noiseScale, 0.01f);
		ImGui::DragFloat("noiseStrength", &noiseData->noiseStrength, 0.01f);
		ImGui::DragInt("octaves", &noiseData->octaves);
		ImGui::DragFloat("roughness", &noiseData->roughness, 0.01f);



		

		ImGui::Separator();
		ImGui::Text("waveData");
		ImGui::Separator();
		if (ImGui::Button("AddWave")) {
			AddWave();
		}
		ImGui::Separator();
		for (size_t i = 0; i < index_; ++i) {
			std::string name = "Wave" + std::to_string(i);
			// 強制的に一つだけ開くように設定
			if (ImGui::TreeNode(name.c_str())) {
				ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&waveData[i].flag));
				ImGui::DragFloat2("waveDirection", &waveData[i].waveDirection.x, 0.1f);
				waveData[i].waveDirection = Normalize(waveData[i].waveDirection);
				ImGui::DragFloat("amplitude", &waveData[i].amplitude, 0.01f);
				ImGui::DragFloat("speed", &waveData[i].speed, 0.01f);
				ImGui::DragFloat("frequency", &waveData[i].frequency, 0.01f);
				ImGui::TreePop();
			}
		}
		ImGui::Separator();
	}
#endif // _DEBUG
}

void Ocean::Draw()
{
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, waveResource->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(8, noiseResource->GetGPUVirtualAddress());

	material->GetCommandListMaterial(0);
	material->GetCommandListTexture(2,0,0,10);

	mesh_->GetCommandList();

	directXCommon_->GetCommandList()->DrawInstanced(UINT(mesh_->vertices.size()), 1, 0, 0);
}

void Ocean::AddWave()
{
	waveData[index_].amplitude = 0.5f;
	waveData[index_].frequency = 2.0f;
	waveData[index_].speed = 1.0f;
	waveData[index_].time = 0;
	waveData[index_].waveDirection = { 0,1 };
	waveData[index_].flag = true;


	index_++;
}
