#include"Sprite.h"
#include"SpriteCommon.h"
#include <iostream>
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/MyGame/MyGame.h"

void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath, bool isTexLoad)
{
	timer_.StartTimer();
	textureFilePath_ = textureFilePath;
	// 引数で受け取ってメンバ変数にする
	this->spriteCommon_ = spriteCommon;


	// インデックスデータ設定
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(2);
	// インデックスリソース生成
	indexResorce_.CreateBufferView(spriteCommon_->GetDxCommon(), indices, indices.size());

	// 頂点データ設定
	vertices.push_back({});
	vertices.push_back({});
	vertices.push_back({});
	vertices.push_back({});
	// 頂点リソース生成
	vbvResorce_.CreateBufferView(spriteCommon_->GetDxCommon(), vertices, vertices.size());


	// マテリアル
	material = std::make_unique<Material>();
	material->Initialize(spriteCommon_->GetDxCommon());
	material->tex_.diffuseFilePath = textureFilePath;
	if (isTexLoad) {
		material->LoadTex();
	}
	//

	// トランスフォーム
	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(spriteCommon_->GetDxCommon());

	// ワールドトランスフォーム初期化
	worldTransform2d.Initialize();

	// SRT設定
	transform.scale = { size.x * worldTransform2d.scale_.x,size.y * worldTransform2d.scale_.y,1.0f };
	transform.rotate = { 0.0f,0.0f,worldTransform2d.rotate_ };
	transform.translate = { worldTransform2d.translate_.x,worldTransform2d.translate_.y,0.0f };


	// 各角
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	/// テクスチャ範囲指定-反映処理-
	const DirectX::TexMetadata& metadata = spriteCommon_->GetDxCommon()->GetTextureManager()->GetMataData(textureFilePath_);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;

	// イメージサイズに合わせる
	AdjusttextureSize();


	timer_.EndTimer();
	timer_.LogTimeSec("InitSprite ", textureFilePath);
}

void Sprite::Update()
{
	worldTransform2d.Update();  // scale_ は Transform の scale 値のみ使う

	float winWidth = static_cast<float>(WinApp::GetClientWidth(false));
	float winHeight = static_cast<float>(WinApp::GetClientHeight(false));


#ifndef _DEBUG
	//winWidth = static_cast<float>(WinApp::GetClientWidth());
	//winHeight = static_cast<float>(WinApp::GetClientHeight());
#endif // _DEBUG


	// anchorPoint を中心にしたジオメトリを "size" で構築
	float left = -anchorPoint.x * size.x;
	float right = (1.0f - anchorPoint.x) * size.x;
	float top = -anchorPoint.y * size.y;
	float bottom = (1.0f - anchorPoint.y) * size.y;

	// 左右反転
	if (isFlipX_) {
		std::swap(left, right);
	}
	// 上下反転
	if (isFlipY_) {
		std::swap(top, bottom);
	}

	// テクスチャUV
	const DirectX::TexMetadata& metadata = spriteCommon_->GetDxCommon()->GetTextureManager()->GetMataData(textureFilePath_);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;

	// 頂点データ更新（サイズから構築）
	vbvResorce_.Data()[0].position = { left,  bottom, 0.0f, 1.0f };
	vbvResorce_.Data()[1].position = { left,  top,    0.0f, 1.0f };
	vbvResorce_.Data()[2].position = { right, bottom, 0.0f, 1.0f };
	vbvResorce_.Data()[3].position = { right, top,    0.0f, 1.0f };

	// UV と normal は変更なし
	for (int i = 0; i < 4; ++i) {
		vbvResorce_.Data()[i].normal = { 0.0f, 0.0f, -1.0f };
	}
	vbvResorce_.Data()[0].texcoord = { tex_left,  tex_bottom };
	vbvResorce_.Data()[1].texcoord = { tex_left,  tex_top };
	vbvResorce_.Data()[2].texcoord = { tex_right, tex_bottom };
	vbvResorce_.Data()[3].texcoord = { tex_right, tex_top };

	// 変換行列
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, winWidth, winHeight, 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldTransform2d.GetConvert2DMatrixTo4x4(), Multiply(viewMatrix, projectionMatrix));

	// シェーダーに送信
	transfomation->UpdateSprite(worldViewProjectionMatrix);
	material->GPUData();
}

void Sprite::UpdateAmimetion(float time)
{
	animeTime_ += MyGame::GameTime();

	if (animeTime_ >= time) {
		textureLeftTop.x += animeSize_.x;
		animeNum_.x++;

		if (animeNum_.x >= maxAnimeNum_.x) {
			textureLeftTop.x = 0;
			animeNum_.x = 0;

			textureLeftTop.y += animeSize_.y;
			animeNum_.y++;

			if (animeNum_.y >= maxAnimeNum_.y) {
				textureLeftTop.y = 0;
				animeNum_.y = 0;
			}
		}
		animeTime_ = 0;
	}
	Update();
}

void Sprite::Draw(PSOType type)
{
	if (isActive_) {
		SpriteTypeDiscrimination(type);

		material->GetCommandListMaterial(0);

		material->GetCommandListTexture(2, 2, 2);


		vbvResorce_.IASetVertexBuffers();
		indexResorce_.IASetIndexBuffer();

		//トランスフォームMatrixResource
		transfomation->GetCommandList(1);


		spriteCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Sprite::GetCommandList() {
	vbvResorce_.IASetVertexBuffers();
	indexResorce_.IASetIndexBuffer();
};

void Sprite::AdjusttextureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = spriteCommon_->GetDxCommon()->GetTextureManager()->GetMataData(textureFilePath_);

	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	size = textureSize;
}

void Sprite::SpriteTypeDiscrimination(PSOType type)
{
	spriteCommon_->DrawCommonSetting(type);
}
