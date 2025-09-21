#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"

#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/Transform/Transfomation/Transfomation.h"
#include"DirectXGame/engine/Material/Material.h"
#include"DirectXGame/engine/Transform/WorldTransform/WorldTransform2d.h"

#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
using namespace Microsoft::WRL;
class SpriteCommon;

class Sprite
{
public:// メンバ関数
	
	// 初期化
	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath, bool isTexLoad = true);
	// 更新
	void Update();
	// アニメーション
	void UpdateAmimetion(float time);

	// 描画
	void Draw(PSOType type = {});

	///位置
	// getter
	const Vector2& GetPosition() const { return worldTransform2d.translate_; }
	// setter
	void SetPosition(const Vector2& position) { worldTransform2d.translate_ = position; }

	// 回転
	float GetRotation() const { return  worldTransform2d.rotate_; }
	void SetRotation(float rotation) { worldTransform2d.rotate_ = rotation; }

	// スケール
	const Vector2& GetScale() const { return worldTransform2d.scale_; }
	void SetScale(const Vector2& size) { worldTransform2d.scale_ = size; }

	// サイズ
	const Vector2& GetSize() const { return size; }
	void SetSize(const Vector2& size) { this->size = size; }
	void SetSize(const float& size) { this->size = this->size * size; }

	// 色
	const Color& GetColor() const { return material->color; }
	void SetColor(const Color& color) { material->color = color; }

	// アンカーポイント
	const Vector2& GetAnchorPoint() const { return anchorPoint; };
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

	// フリップX
	const bool& GetIsFlipX() const { return isFlipX_; };
	void SetIsFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	// フリップY
	const bool& GetIsFlipY() const { return isFlipY_; };
	void SetIsFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

	//テクスチャ左上座標
	const Vector2& GetextureLeftTop() const { return textureLeftTop; };
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	// テクスチャ切り出し座標
	const Vector2& GetTextureSize() const { return textureSize; };
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

	// アニメサイズ
	void SetAnimeSize(const Vector2& animeSize) { animeSize_ = animeSize; }
	// アニメ切り替え数
	void SetMaxAnimeNum(const Vector2& animeNum) { maxAnimeNum_ = animeNum; }
	// ピクセル補間
	void SetIsPixelInterpolation(bool is) { isPixelInterpolation_ = is; }

	// 描画させるか
	void SetActive(const bool& isActive) { isActive_ = isActive; }
	bool GetActive() const { return isActive_; }

	// フェードイン
	void SetIsFadeIn(const bool& isFadeIn) { isFadeIn_ = isFadeIn; };
	void SetIsFadeOut(const bool& isFadeOut) { isFadeOut_ = isFadeOut; };

	void SetName(std::string name) { name_ = name; }

	std::string GetName() { return name_; }

	void SetTexture(const std::string texture) { material->tex_.diffuseFilePath = texture; };

	WorldTransform2d& GetWorldTransform2d() { return worldTransform2d; }

	void GetCommandList();

private:

	// テクスチャサイズをイメージに合わせる
	void AdjusttextureSize();

	void SpriteTypeDiscrimination(PSOType type);

private:
	// スプライト用
	SpriteCommon* spriteCommon_ = nullptr;

	// インデクスデータ
	IndexBuffer<uint32_t> indexResorce_;
	std::vector<uint32_t> indices;

	VertexBuffer<VertexData> vbvResorce_;
	std::vector<VertexData> vertices;




	// トランスフォーム
	std::unique_ptr<Transfomation>transfomation = nullptr;
	// マテリアル
	std::unique_ptr<Material> material = nullptr;
	// テクスチャパス
	std::string textureFilePath_;
	//テクスチャ番号
	uint32_t textureIndex = 0;

	std::string name_ = "NoName";
private:

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };	// トランスフォーム

	WorldTransform2d worldTransform2d;

	Vector2 size = { 640.0f,360.0f };											// サイズ													
	bool isActive_ = true;
private:

	// アニメーション関係

	Vector2 maxAnimeNum_;
	Vector2 animeNum_;
	float animeTime_;
	Vector2 animeSize_;

	bool isPixelInterpolation_ = true;// 表示させるか

	// フェードインフラグ
	bool isFadeIn_ = false;
	// フェードアウトフラグ
	bool isFadeOut_ = false;


private: // テクスチャいじいじ
	DebugTimer timer_;

	// アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };
	// 左右フリップ
	bool isFlipX_ = false;
	// 上下フリップ
	bool isFlipY_ = false;
	// テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	// テクスチャ切り出し座標
	Vector2 textureSize = { 100.0f,100.0f };
};

