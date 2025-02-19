#include "Ocean.h"
#include "DirectXGame/engine/base/TextureManager.h"
#include "OceanManager.h"
#include "imgui.h"

void Ocean::Initialize(Vector2 renge)
{
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

	mesh_->Initialize(OceanManager::GetInstance()->GetDxCommon());






	
	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(OceanManager::GetInstance()->GetDxCommon());

	
	material = std::make_unique<Material>();
	material->Initialize(OceanManager::GetInstance()->GetDxCommon());
	material->tex_.diffuseFilePath = "resources/Texture/Image.png";
	material->LoadTex();
	material->color = { 0,0,1,1.0f };


	noiseResource = OceanManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(NoiseData));
	noiseResource->Map(0, nullptr, reinterpret_cast<void**>(&noiseData));

	noiseData->noiseScale = 1.0f;
	noiseData->noiseStrength = 1.0f;
	noiseData->octaves = 1;
	noiseData->roughness = 1.0f;



	waveResource = OceanManager::GetInstance()->GetDxCommon()->CreateBufferResource(sizeof(WaveParameters) * 3);
	waveResource->Map(0, nullptr, reinterpret_cast<void**>(&waveData));

	waveData->amplitude = 1.0f;
	waveData->frequency = 2.0f;
	waveData->speed = 1.0f;
	waveData->time = 0;
	waveData->waveDirection = { 0,1 };
	

	

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
		ImGui::DragFloat2("waveDirection", &waveData->waveDirection.x, 0.1f);
		waveData->waveDirection = Normalize(waveData->waveDirection);
		ImGui::DragFloat("amplitude", &waveData->amplitude, 0.01f);
		ImGui::DragFloat("speed", &waveData->speed, 0.01f);
		ImGui::DragFloat("frequency", &waveData->frequency, 0.01f);


		
		
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
	OceanManager::GetInstance()->DrawCommonSetting();

	LightManager::GetInstance()->DrawLight();

	OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(7, waveResource->GetGPUVirtualAddress());
	OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(8, noiseResource->GetGPUVirtualAddress());

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


	OceanManager::GetInstance()->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(mesh_->vertices.size()), 1, 0, 0);


}
