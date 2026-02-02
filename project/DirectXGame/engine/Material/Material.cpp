#include "Material.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::Material::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	dxCommon_ = dxcommon;

	// リソース生成
	cbResource_ = std::make_unique<ConstantBuffer<Material::DataGPU>>();
	cbResource_->CreateBuffer(dxCommon_);

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
	// テクスチャのバインド
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(indexDiffuse, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.diffuseFilePath));
	
	// ノーマルマップ
	if (GetMaterialInstance().useNormalMap_) {

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(normalIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(9, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
	}
	// スペキュラーマップ
	if (GetMaterialInstance().useSpeculerMap_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(speculerIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.speculerFilePath));
	}

	// 環境マップ
	if (GetMaterialInstance().useEnvironment_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(environmentIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.environmentFilePath));
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


	if (GetMaterialInstance().useNormalMap_) {
		cbResource_->Data()->useNormalMap = GetMaterialInstance().useNormalMap_;
	}
	if (GetMaterialInstance().useSpeculerMap_) {
		cbResource_->Data()->useSpeculerMap = GetMaterialInstance().useSpeculerMap_;
	}
}

void Engine::Material::LoadTex()
{

	// .objの参照しているテクスチャファイル読み込み
	dxCommon_->GetTextureManager()->LoadTexture(tex_.diffuseFilePath);
	// 読み込んだテクスチャの番号を取得
	tex_.diffuseIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.diffuseFilePath);

	if (tex_.normalFilePath == "") {
		GetMaterialInstance().useNormalMap_ = false;
	}
	else {
		GetMaterialInstance().useNormalMap_ = true;
	}

	// ノーマルマップ
	if (GetMaterialInstance().useNormalMap_) {
		dxCommon_->GetTextureManager()->LoadTexture(tex_.normalFilePath);

		tex_.normalIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.normalFilePath);
	}

	
	if (tex_.speculerFilePath == "") {
		GetMaterialInstance().useSpeculerMap_ = false;
	}
	else {
		GetMaterialInstance().useSpeculerMap_ = true;
	}
	
	// スペキュラーマップ
	if (GetMaterialInstance().useSpeculerMap_) {

		dxCommon_->GetTextureManager()->LoadTexture(tex_.speculerFilePath);

		tex_.speculerIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.speculerFilePath);
	}

	if (tex_.environmentFilePath == "") {
		GetMaterialInstance().useEnvironment_ = false;
	}
	else {
		GetMaterialInstance().useEnvironment_ = true;

		dxCommon_->GetTextureManager()->LoadTexture(tex_.environmentFilePath);

		tex_.environmentIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.environmentFilePath);
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

	if (materialInstance.useNormalMap_) {
		cbResourcePtr->Data()->useNormalMap = materialInstance.useNormalMap_;
	}
	if (materialInstance.useSpeculerMap_) {
		cbResourcePtr->Data()->useSpeculerMap = materialInstance.useSpeculerMap_;
	}



	cbResourcePtr->SetGraphicsRootConstantBufferView(0);
}
