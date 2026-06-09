#include "Material.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::Material::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	this->dxCommon = dxcommon;

	// リソース生成
	cbResource_ = std::make_unique<ConstantBuffer<Material::DataGPU>>();
	cbResource_->CreateBuffer(dxCommon);

	// SRTと色設定
	GetMaterialInstance().transform.scale = { 1.0f,1.0f,1.0f };
	GetMaterialInstance().transform.translate = { 0,0,0 };
	GetMaterialInstance().transform.rotate = { 0,0,0 };
	GetMaterialInstance().color = { 1,1,1,1 };

	// 反射
	GetMaterialInstance().shininess_ = 20.0f;
	GetMaterialInstance().enableLighting_ = true;	// ライティング
	GetMaterialInstance().useNormalMap_ = false;	// ノーマルマップ
	GetMaterialInstance().useSpeculerMap_ = false;// スペキュラーマップ
	GetMaterialInstance().useEnvironment_ = false;// 環境マップ

	// GPUデータ
	GPUData();
}

void Engine::Material::GetCommandListMaterial(int index)
{
	cbResource_->SetGraphicsRootConstantBufferView(index);
}

void Engine::Material::GetCommandListTexture(int indexDiffuse, int normalIndex, int speculerIndex, int environmentIndex)
{
	TextureManager* tex = dxCommon->GetTextureManager();
	auto command = dxCommon->GetCommandList();

	// テクスチャのバインド
	command->SetGraphicsRootDescriptorTable(indexDiffuse, tex->GetSrvHandleGPU(tex_.diffuseFilePath));
	
	// ノーマルマップ
	if (GetMaterialInstance().useNormalMap_) {

		command->SetGraphicsRootDescriptorTable(normalIndex, tex->GetSrvHandleGPU(tex_.normalFilePath));
		command->SetGraphicsRootDescriptorTable(9, tex->GetSrvHandleGPU(tex_.normalFilePath));
	}
	// スペキュラーマップ
	if (GetMaterialInstance().useSpeculerMap_) {
		command->SetGraphicsRootDescriptorTable(speculerIndex, tex->GetSrvHandleGPU(tex_.speculerFilePath));
	}

	// 環境マップ
	if (GetMaterialInstance().useEnvironment_) {
		command->SetGraphicsRootDescriptorTable(environmentIndex, tex->GetSrvHandleGPU(tex_.environmentFilePath));
	}
}



void Engine::Material::GPUData()
{

	// 各データをGPUに送る
	cbResource_->Data()->environmentCoefficient = GetMaterialInstance().environmentCoefficient_;
	cbResource_->Data()->enableLighting = GetMaterialInstance().enableLighting_;
	cbResource_->Data()->shininess = GetMaterialInstance().shininess_;
	cbResource_->Data()->useLig = GetMaterialInstance().useLig_;
	cbResource_->Data()->alpha = GetMaterialInstance().alpha_;
	cbResource_->Data()->color = GetMaterialInstance().color;
	cbResource_->Data()->uvTransform = MakeAffineMatrix(GetMaterialInstance().transform.scale, GetMaterialInstance().transform.rotate, GetMaterialInstance().transform.translate);
	cbResource_->Data()->alphaClipping = GetMaterialInstance().alphaClipping_;
	// ノーマル/スペキュラマップの使用フラグは、OFFになった時もGPUへ必ず反映する
	cbResource_->Data()->useNormalMap = GetMaterialInstance().useNormalMap_;
	cbResource_->Data()->useSpeculerMap = GetMaterialInstance().useSpeculerMap_;
}

void Engine::Material::LoadTex()
{
	TextureManager* tex = dxCommon->GetTextureManager();

	// .objの参照しているテクスチャファイル読み込み
	tex->LoadTexture(tex_.diffuseFilePath);
	// 読み込んだテクスチャの番号を取得
	tex_.diffuseIndex = tex->GetTextureIndexByFilePath(tex_.diffuseFilePath);

	if (tex_.normalFilePath == "") {
		GetMaterialInstance().useNormalMap_ = false;
	}
	else {
		GetMaterialInstance().useNormalMap_ = true;
	}

	// ノーマルマップ
	if (GetMaterialInstance().useNormalMap_) {
		tex->LoadTexture(tex_.normalFilePath);

		tex_.normalIndex = tex->GetTextureIndexByFilePath(tex_.normalFilePath);
	}

	
	if (tex_.speculerFilePath == "") {
		GetMaterialInstance().useSpeculerMap_ = false;
	}
	else {
		GetMaterialInstance().useSpeculerMap_ = true;
	}
	
	// スペキュラーマップ
	if (GetMaterialInstance().useSpeculerMap_) {

		tex->LoadTexture(tex_.speculerFilePath);

		tex_.speculerIndex = tex->GetTextureIndexByFilePath(tex_.speculerFilePath);
	}

	if (tex_.environmentFilePath == "") {
		GetMaterialInstance().useEnvironment_ = false;
	}
	else {
		GetMaterialInstance().useEnvironment_ = true;

		tex->LoadTexture(tex_.environmentFilePath);

		tex_.environmentIndex = tex->GetTextureIndexByFilePath(tex_.environmentFilePath);
	}
}

void Engine::Material::SetGPUMaterialInstance(const MaterialInstance& materialInstance, ConstantBuffer < Material::DataGPU>* cbResourcePtr)
{
	// 各データをGPUに送る
	cbResourcePtr->Data()->environmentCoefficient = materialInstance.environmentCoefficient_;
	cbResourcePtr->Data()->enableLighting = materialInstance.enableLighting_;
	cbResourcePtr->Data()->shininess = materialInstance.shininess_;
	cbResourcePtr->Data()->useLig = materialInstance.useLig_;
	cbResourcePtr->Data()->alpha = materialInstance.alpha_;
	cbResourcePtr->Data()->alphaClipping = materialInstance.alphaClipping_;
	cbResourcePtr->Data()->color = materialInstance.color;
	cbResourcePtr->Data()->uvTransform = MakeAffineMatrix(materialInstance.transform.scale, materialInstance.transform.rotate, materialInstance.transform.translate);
	// ノーマル/スペキュラマップの使用フラグは、OFFになった時もGPUへ必ず反映する
	cbResourcePtr->Data()->useNormalMap = materialInstance.useNormalMap_;
	cbResourcePtr->Data()->useSpeculerMap = materialInstance.useSpeculerMap_;



	cbResourcePtr->SetGraphicsRootConstantBufferView(0);
}
