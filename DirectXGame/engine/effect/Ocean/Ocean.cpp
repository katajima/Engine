#include "Ocean.h"
#include "DirectXGame/engine/base/TextureManager.h"
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

	mesh_->Initialize(directXCommon_->GetModelManager()->GetModelCommon());






	
	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(directXCommon_);

	
	material = std::make_unique<Material>();
	material->Initialize(entity3dManager_->GetOceanManager()->GetDxCommon());
	material->tex_.diffuseFilePath = "resources/Texture/Image.png";
	material->LoadTex();
	material->color = { 0,0,1,1.0f };


	noiseResource = directXCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(NoiseData));
	noiseResource->Map(0, nullptr, reinterpret_cast<void**>(&noiseData));

	noiseData->noiseScale = 10.0f;
	noiseData->noiseStrength = 1.0f;
	noiseData->octaves = 37;
	noiseData->roughness = 10.0f;



	waveResource = directXCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(WaveParameters) * 3);
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

	

	//transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,10.0f,0.0f} };
}

void Ocean::Update()
{
#ifdef _DEBUG



	ImGui::Begin("engine");


	if (ImGui::CollapsingHeader("Ocean")) {
		ImGui::Text("SRT");
		ImGui::Separator();
		ImGui::DragFloat3("translate", &transform.translate.x, 0.01f);
		ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
		ImGui::DragFloat3("scale", &transform.scale.x, 0.01f);
		ImGui::Separator();
		ImGui::Text("material");
		ImGui::Separator();
		ImGui::ColorEdit4("color", &material->color.r);
		ImGui::Separator();
		ImGui::Text("noiseData");
		ImGui::Separator();
		ImGui::DragFloat("noiseScale", &noiseData->noiseScale, 0.01f);
		ImGui::DragFloat("noiseStrength", &noiseData->noiseStrength, 0.01f);
		ImGui::DragInt("octaves", &noiseData->octaves);
		ImGui::DragFloat("roughness", &noiseData->roughness, 0.01f);



		ImGui::Separator();
		ImGui::Text("waveData");
		ImGui::Separator();

		ImGui::Checkbox("Enable", reinterpret_cast<bool*>(&waveData[0].flag));
		ImGui::DragFloat2("waveDirection", &waveData[0].waveDirection.x, 0.1f);
		waveData[0].waveDirection = Normalize(waveData[0].waveDirection);
		ImGui::DragFloat("amplitude", &waveData[0].amplitude, 0.01f);
		ImGui::DragFloat("speed", &waveData[0].speed, 0.01f);
		ImGui::DragFloat("frequency", &waveData[0].frequency, 0.01f);

		ImGui::Separator();

		ImGui::Checkbox("Enable1", reinterpret_cast<bool*>(&waveData[1].flag));
		ImGui::DragFloat2("waveDirection1", &waveData[1].waveDirection.x, 0.1f);
		waveData[1].waveDirection = Normalize(waveData[1].waveDirection);
		ImGui::DragFloat("amplitude1", &waveData[1].amplitude, 0.01f);
		ImGui::DragFloat("speed1", &waveData[1].speed, 0.01f);
		ImGui::DragFloat("frequency1", &waveData[1].frequency, 0.01f);

		ImGui::Separator();

		ImGui::Checkbox("Enable2", reinterpret_cast<bool*>(&waveData[2].flag));
		ImGui::DragFloat2("waveDirection2", &waveData[2].waveDirection.x, 0.1f);
		waveData[2].waveDirection = Normalize(waveData[2].waveDirection);
		ImGui::DragFloat("amplitude2", &waveData[2].amplitude, 0.01f);
		ImGui::DragFloat("speed2", &waveData[2].speed, 0.01f);
		ImGui::DragFloat("frequency2", &waveData[2].frequency, 0.01f);


		
		
	}
	ImGui::End();
#endif // _DEBUG



	waveData->time += 1.0f / 60.0f;
	
	// ワールド行列の計算
	mat_ = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	
	transfomation->Update(camera,mat_);

	material->GPUData();
}

void Ocean::Draw()
{
	entity3dManager_->GetOceanManager()->DrawCommonSetting();


	entity3dManager_->GetLightManager()->DrawLight();
	
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, waveResource->GetGPUVirtualAddress());
	directXCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(8, noiseResource->GetGPUVirtualAddress());

	transfomation->GetCommandList(1);
	transfomation->GetCommandList(9);

	camera->GetCommandList(4);

	material->GetCommandListMaterial(0);

	material->GetCommandListTexture(2);

	
	// 頂点バッファの設定
	//OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	// インデックスバッファの設定
	//OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	mesh_->GetCommandList();

	// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
	//OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);


	directXCommon_->GetCommandList()->DrawInstanced(UINT(mesh_->vertices.size()), 1, 0, 0);


}
