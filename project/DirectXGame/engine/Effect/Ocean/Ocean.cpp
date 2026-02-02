#include "Ocean.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "OceanManager.h"
#include "imgui.h"



void Engine::Ocean::Initialize(Engine::Entity3DManager* entity3dManager,Vector2 renge)
{
	entity3dManager_ = entity3dManager;	// エンティティ3d

	// DX共通クラス
	directXCommon_ = entity3dManager_->GetOceanManager()->GetDxCommon();

	// 範囲
	renge_.renge = renge;

	mesh_ = std::make_unique<ModelMesh>();
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





	// マテリアル生成
	material = std::make_unique<Material>();
	material->Initialize(entity3dManager_->GetOceanManager()->GetDxCommon());
	material->tex_.diffuseFilePath = "resources/Texture/Image.png";
	material->tex_.environmentFilePath = "resources/Texture/hdr/sky.dds";
	material->LoadTex();
	material->GetMaterialInstance().color = { 0,0,1,1.0f };


	// ノイズリソース生成
	cbNoiseResource_.CreateBuffer(directXCommon_);
	cbNoiseResource_.Data()[0].noiseScale = 10.0f;
	cbNoiseResource_.Data()[0].noiseStrength = 1.0f;
	cbNoiseResource_.Data()[0].octaves = 37;
	cbNoiseResource_.Data()[0].roughness = 10.0f;


	// ウェーブリソース生成
	cbWaveResource_.CreateBuffer(directXCommon_,10);
	cbWaveResource_.Data()[0].amplitude = 1.500f;
	cbWaveResource_.Data()[0].frequency = 3.340f;
	cbWaveResource_.Data()[0].speed = 1.0f;
	cbWaveResource_.Data()[0].time = 0;
	cbWaveResource_.Data()[0].waveDirection = { 0,1 };
	cbWaveResource_.Data()[0].flag = true;
	
	cbWaveResource_.Data()[1].amplitude = 1.0f;
	cbWaveResource_.Data()[1].frequency = 2.0f;
	cbWaveResource_.Data()[1].speed = 1.0f;
	cbWaveResource_.Data()[1].time = 0;
	cbWaveResource_.Data()[1].waveDirection = { 0.5f,0.5f };
	cbWaveResource_.Data()[1].flag = true;
	
	cbWaveResource_.Data()[2].amplitude = 0.5f;
	cbWaveResource_.Data()[2].frequency = 2.0f;
	cbWaveResource_.Data()[2].speed = 1.0f;
	cbWaveResource_.Data()[2].time = 0;
	cbWaveResource_.Data()[2].waveDirection = { 0,1 };
	cbWaveResource_.Data()[2].flag = true;

	index_ = 3;
}

void Engine::Ocean::Update()
{
	for (size_t i = 0; i < index_; ++i) {
		cbWaveResource_.Data()[i].time += 1.0f / 60.0f;
	}
	material->GPUData();
}

void Engine::Ocean::UpdateImgui()
{
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Wave")) {
		ImGui::Text("noiseData");
		ImGui::Separator();
		ImGui::DragFloat("noiseScale", &cbNoiseResource_.Data()[0].noiseScale, 0.01f);
		ImGui::DragFloat("noiseStrength", &cbNoiseResource_.Data()[0].noiseStrength, 0.01f);
		ImGui::DragInt("octaves", &cbNoiseResource_.Data()[0].octaves);
		ImGui::DragFloat("roughness", &cbNoiseResource_.Data()[0].roughness, 0.01f);



		

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
				ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&cbWaveResource_.Data()[i].flag));
				ImGui::DragFloat2("waveDirection", &cbWaveResource_.Data()[i].waveDirection.x, 0.1f);
				cbWaveResource_.Data()[i].waveDirection = Normalize(cbWaveResource_.Data()[i].waveDirection);
				ImGui::DragFloat("amplitude", &cbWaveResource_.Data()[i].amplitude, 0.01f);
				ImGui::DragFloat("speed", &cbWaveResource_.Data()[i].speed, 0.01f);
				ImGui::DragFloat("frequency", &cbWaveResource_.Data()[i].frequency, 0.01f);
				ImGui::TreePop();
			}
		}
		ImGui::Separator();
	}
#endif // _DEBUG
}

void Engine::Ocean::Draw()
{

	// ウェーブデータ
	cbWaveResource_.SetGraphicsRootConstantBufferView(7);
	// ノイズデータ
	cbNoiseResource_.SetGraphicsRootConstantBufferView(8);

	material->GetCommandListMaterial(0);
	material->GetCommandListTexture(2,0,0,10);

	mesh_->GetCommandList();

	directXCommon_->GetCommandList()->DrawInstanced(UINT(mesh_->vertices.size()), 1, 0, 0);
}

void Engine::Ocean::AddWave()
{
	cbWaveResource_.Data()[index_].amplitude = 0.5f;
	cbWaveResource_.Data()[index_].frequency = 2.0f;
	cbWaveResource_.Data()[index_].speed = 1.0f;
	cbWaveResource_.Data()[index_].time = 0;
	cbWaveResource_.Data()[index_].waveDirection = { 0,1 };
	cbWaveResource_.Data()[index_].flag = true;


	index_++;
}
