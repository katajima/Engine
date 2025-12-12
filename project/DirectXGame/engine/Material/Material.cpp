#include "Material.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::Material::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	dxCommon_ = dxcommon;

	// リソース生成
	cbResource_.CreateBuffer(dxCommon_);

	// SRTと色設定
	transform.scale = { 1.0f,1.0f,1.0f };
	transform.translate = { 0,0,0 };
	transform.rotate = { 0,0,0 };
	color = { 1,1,1,1 };

	// 反射
	shininess_ = 20.0f;
	enableLighting_ = true;	// ライティング
	useNormalMap_ = false;	// ノーマルマップ
	useSpeculerMap_ = false;// スペキュラーマップ
	useEnvironment_ = false;// 環境マップ

	// GPUデータ
	GPUData();
}

void Engine::Material::GetCommandListMaterial(int index)
{
	cbResource_.SetGraphicsRootConstantBufferView(0);
}

void Engine::Material::GetCommandListTexture(int indexDiffuse, int normalIndex, int speculerIndex, int environmentIndex)
{
	// テクスチャのバインド
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(indexDiffuse, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.diffuseFilePath));
	
	// ノーマルマップ
	if (useNormalMap_) {

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(normalIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(9, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
	}
	// スペキュラーマップ
	if (useSpeculerMap_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(speculerIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.speculerFilePath));
	}

	// 環境マップ
	if (useEnvironment_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(environmentIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.environmentFilePath));
	}
}



void Engine::Material::GPUData()
{

	// 各データをGPUに送る
	cbResource_.Data()->environmentCoefficient = environmentCoefficient_;
	cbResource_.Data()->enableLighting = enableLighting_;
	cbResource_.Data()->shininess = shininess_;
	cbResource_.Data()->useLig = useLig_;
	cbResource_.Data()->alpha = alpha_;
	if (useNormalMap_) {
		cbResource_.Data()->useNormalMap = useNormalMap_;
	}
	if (useSpeculerMap_) {
		cbResource_.Data()->useSpeculerMap = useSpeculerMap_;
	}

	cbResource_.Data()->alphaClipping = alphaClipping_;

	cbResource_.Data()->color = color;
	//cbResource_.Data()
	cbResource_.Data()->uvTransform = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
}

void Engine::Material::LoadTex()
{

	// .objの参照しているテクスチャファイル読み込み
	dxCommon_->GetTextureManager()->LoadTexture(tex_.diffuseFilePath);
	// 読み込んだテクスチャの番号を取得
	tex_.diffuseIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.diffuseFilePath);

	if (tex_.normalFilePath == "") {
		useNormalMap_ = false;
	}
	else {
		useNormalMap_ = true;
	}

	// ノーマルマップ
	if (useNormalMap_) {
		dxCommon_->GetTextureManager()->LoadTexture(tex_.normalFilePath);

		tex_.normalIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.normalFilePath);
	}

	
	if (tex_.speculerFilePath == "") {
		useSpeculerMap_ = false;
	}
	else {
		useSpeculerMap_ = true;
	}
	
	// スペキュラーマップ
	if (useSpeculerMap_) {

		dxCommon_->GetTextureManager()->LoadTexture(tex_.speculerFilePath);

		tex_.speculerIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.speculerFilePath);
	}

	if (tex_.environmentFilePath == "") {
		useEnvironment_ = false;
	}
	else {
		useEnvironment_ = true;

		dxCommon_->GetTextureManager()->LoadTexture(tex_.environmentFilePath);

		tex_.environmentIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath(tex_.environmentFilePath);
	}



}
