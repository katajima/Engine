#include "Material.h"
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Material::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;


	cbResource_.CreateBuffer(dxCommon_);

	transform.scale = { 1.0f,1.0f,1.0f };
	transform.translate = { 0,0,0 };
	transform.rotate = { 0,0,0 };
	color = { 1,1,1,1 };

	shininess_ = 20.0f;
	enableLighting_ = true;
	useNormalMap_ = false;
	useSpeculerMap_ = false;
	useEnvironment_ = false;

	// GPUデータ
	GPUData();
}

void Material::GetCommandListMaterial(int index)
{
	cbResource_.SetGraphicsRootConstantBufferView(0);
}

void Material::GetCommandListTexture(int indexDiffuse, int normalIndex, int speculerIndex, int environmentIndex)
{
	// テクスチャのバインド
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(indexDiffuse, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.diffuseFilePath));
	if (useNormalMap_) {

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(normalIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(9, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.normalFilePath));
	}
	if (useSpeculerMap_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(speculerIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.speculerFilePath));
	}

	// 環境マップ
	if (useEnvironment_) {
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(environmentIndex, dxCommon_->GetTextureManager()->GetSrvHandleGPU(tex_.environmentFilePath));
	}
}



void Material::GPUData()
{
	

	cbResource_.Data()->environmentCoefficient = environmentCoefficient_;
	cbResource_.Data()->enableLighting = enableLighting_;
	cbResource_.Data()->shininess = shininess_;
	cbResource_.Data()->useLig = useLig_;

	if (useNormalMap_) {
		cbResource_.Data()->useNormalMap = useNormalMap_;
	}
	if (useSpeculerMap_) {
		cbResource_.Data()->useSpeculerMap = useSpeculerMap_;
	}

	cbResource_.Data()->alphaClipping = alphaClipping_;

	cbResource_.Data()->color = color;

	cbResource_.Data()->uvTransform = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
}

void Material::LoadTex()
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
