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

//前方宣言
class SpriteCommon;

/// <summary>
/// スプライトクラス
/// </summary>
class Sprite
{
public:// メンバ関数
	
	/// <summary>
	/// スプライトの共通設定を初期化します。
	/// </summary>
	/// <param name="spriteCommon">初期化するスプライト共通設定のポインタ。</param>
	/// <param name="textureFilePath">テクスチャファイルのパス。</param>
	/// <param name="isTexLoad">テクスチャをロードするかどうかを指定するフラグ (デフォルトは true)。</param>
	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath, bool isTexLoad = true);
	// 更新
	void Update();
	// アニメーション(デルタ時間)
	void UpdateAmimetion(float time);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="type">うつり方</param>
	void Draw(PSOType type = {});


	// getter

	//位置取得
	const Vector2& GetPosition() const { return worldTransform2d.translate_; }
	// 位置設定
	void SetPosition(const Vector2& position) { worldTransform2d.translate_ = position; }

	// 回転取得
	float GetRotation() const { return  worldTransform2d.rotate_; }
	// 回転設定
	void SetRotation(float rotation) { worldTransform2d.rotate_ = rotation; }

	// スケール取得
	const Vector2& GetScale() const { return worldTransform2d.scale_; }
	// スケール設定
	void SetScale(const Vector2& size) { worldTransform2d.scale_ = size; }

	// サイズ取得
	const Vector2& GetSize() const { return size; }
	
	// サイズ設定
	void SetSize(const Vector2& size) { this->size = size; }
	// サイズ設定(size.x * size,size.y * size)
	void SetSize(const float& size) { this->size = this->size * size; }

	// 色取得
	const Color& GetColor() const { return material->color; }
	// 色設定
	void SetColor(const Color& color) { material->color = color; }

	// アンカーポイント取得
	const Vector2& GetAnchorPoint() const { return anchorPoint; };
	// アンカーポイント設定
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

	// フリップX取得
	const bool& GetIsFlipX() const { return isFlipX_; };
	// フリップX設定
	void SetIsFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
	// フリップY取得
	const bool& GetIsFlipY() const { return isFlipY_; };
	// フリップY設定
	void SetIsFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

	// テクスチャ左上座標取得
	const Vector2& GetextureLeftTop() const { return textureLeftTop; };
	// テクスチャ左上座標設定
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	// テクスチャ切り出し座標取得
	const Vector2& GetTextureSize() const { return textureSize; };
	// テクスチャ切り出し座標設定
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

	// アニメサイズ設定
	void SetAnimeSize(const Vector2& animeSize) { animeSize_ = animeSize; }
	// アニメ切り替え数設定
	void SetMaxAnimeNum(const Vector2& animeNum) { maxAnimeNum_ = animeNum; }
	// ピクセル補間設定
	void SetIsPixelInterpolation(bool is) { isPixelInterpolation_ = is; }

	// 描画させるか設定
	void SetActive(const bool& isActive) { isActive_ = isActive; }
	// 描画されているか取得
	bool GetActive() const { return isActive_; }

	// フェードイン設定
	void SetIsFadeIn(const bool& isFadeIn) { isFadeIn_ = isFadeIn; };
	// フェードアウト設定
	void SetIsFadeOut(const bool& isFadeOut) { isFadeOut_ = isFadeOut; };
	// 名前設定
	void SetName(std::string name) { name_ = name; }
	// 名前取得
	std::string GetName() { return name_; }
	// テクスチャ設定
	void SetTexture(const std::string texture) { material->tex_.diffuseFilePath = texture; };
	// トランスフォーム取得
	WorldTransform2d& GetWorldTransform2d() { return worldTransform2d; }
	// コマンドリスト(頂点処理)取得
	void GetCommandList();

private:

	// テクスチャサイズをイメージに合わせる
	void AdjusttextureSize();
	// スプライトのうつり方処理
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

