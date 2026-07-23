#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"

#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/Transform/Transformation/Transformation.h"
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


namespace Engine {
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
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// アニメーション(デルタ時間)
		/// </summary>
		void UpdateAmimetion(float time);

		/// <summary>
		/// 描画
		/// </summary>
		/// <param name="type">うつり方</param>
		void Draw(PSOType type = {});


		// getter

		/// <summary>
		/// 位置取得
		/// </summary>
		const Vector2& GetPosition() const { return worldTransform2d.translate_; }
		/// <summary>
		/// 位置設定
		/// </summary>
		void SetPosition(const Vector2& position) { worldTransform2d.translate_ = position; }

		/// <summary>
		/// 回転取得
		/// </summary>
		float GetRotation() const { return  worldTransform2d.rotate_; }
		/// <summary>
		/// 回転設定
		/// </summary>
		void SetRotation(float rotation) { worldTransform2d.rotate_ = rotation; }

		/// <summary>
		/// スケール取得
		/// </summary>
		const Vector2& GetScale() const { return worldTransform2d.scale_; }
		/// <summary>
		/// スケール設定
		/// </summary>
		void SetScale(const Vector2& size) { worldTransform2d.scale_ = size; }

		/// <summary>
		/// サイズ取得
		/// </summary>
		const Vector2& GetSize() const { return size; }

		/// <summary>
		/// サイズ設定
		/// </summary>
		void SetSize(const Vector2& size) { this->size = size; }
		/// <summary>
		/// サイズ設定(size.x * size,size.y * size)
		/// </summary>
		void SetSize(const float& size) { this->size = this->size * size; }

		/// <summary>
		/// 色取得
		/// </summary>
		const Color& GetColor() const { return material->GetMaterialInstance().color; }
		/// <summary>
		/// 色設定
		/// </summary>
		void SetColor(const Color& color) { material->GetMaterialInstance().color = color; }

		/// <summary>
		/// アンカーポイント取得
		/// </summary>
		const Vector2& GetAnchorPoint() const { return anchorPoint; };
		/// <summary>
		/// アンカーポイント設定
		/// </summary>
		void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

		/// <summary>
		/// フリップX取得
		/// </summary>
		const bool& GetIsFlipX() const { return isFlipX_; };
		/// <summary>
		/// フリップX設定
		/// </summary>
		void SetIsFlipX(const bool& isFlipX) { this->isFlipX_ = isFlipX; }
		/// <summary>
		/// フリップY取得
		/// </summary>
		const bool& GetIsFlipY() const { return isFlipY_; };
		/// <summary>
		/// フリップY設定
		/// </summary>
		void SetIsFlipY(const bool& isFlipY) { this->isFlipY_ = isFlipY; }

		/// <summary>
		/// テクスチャ左上座標取得
		/// </summary>
		const Vector2& GetextureLeftTop() const { return textureLeftTop; };
		/// <summary>
		/// テクスチャ左上座標設定
		/// </summary>
		void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
		/// <summary>
		/// テクスチャ切り出し座標取得
		/// </summary>
		const Vector2& GetTextureSize() const { return textureSize; };
		/// <summary>
		/// テクスチャ切り出し座標設定
		/// </summary>
		void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

		/// <summary>
		/// アニメサイズ設定
		/// </summary>
		void SetAnimeSize(const Vector2& animeSize) { animeSize_ = animeSize; }
		/// <summary>
		/// アニメ切り替え数設定
		/// </summary>
		void SetMaxAnimeNum(const Vector2& animeNum) { maxAnimeNum_ = animeNum; }
		/// <summary>
		/// ピクセル補間設定
		/// </summary>
		void SetIsPixelInterpolation(bool is) { isPixelInterpolation_ = is; }

		/// <summary>
		/// 描画させるか設定
		/// </summary>
		void SetActive(const bool& isActive) { isActive_ = isActive; }
		/// <summary>
		/// 描画されているか取得
		/// </summary>
		bool GetActive() const { return isActive_; }

		/// <summary>
		/// フェードイン設定
		/// </summary>
		void SetIsFadeIn(const bool& isFadeIn) { isFadeIn_ = isFadeIn; };
		/// <summary>
		/// フェードアウト設定
		/// </summary>
		void SetIsFadeOut(const bool& isFadeOut) { isFadeOut_ = isFadeOut; };
		/// <summary>
		/// 名前設定
		/// </summary>
		void SetName(std::string name) { name_ = name; }
		/// <summary>
		/// 名前取得
		/// </summary>
		std::string GetName() { return name_; }
		/// <summary>
		/// テクスチャ設定
		/// </summary>
		void SetTexture(const std::string texture) { material->tex_.diffuseFilePath = texture; };
		/// <summary>
		/// トランスフォーム取得
		/// </summary>
		WorldTransform2d& GetWorldTransform2d() { return worldTransform2d; }
		/// <summary>
		/// コマンドリスト(頂点処理)取得
		/// </summary>
		void GetCommandList();

	private:

		/// <summary>
		/// テクスチャサイズをイメージに合わせる
		/// </summary>
		void AdjusttextureSize();
		/// <summary>
		/// スプライトのうつり方処理
		/// </summary>
		void SpriteTypeDiscrimination(PSOType type);

	private:
		// スプライト用
		SpriteCommon* spriteCommon = nullptr;

		// インデクスデータ
		IndexBuffer<uint32_t> indexResource_;
		std::vector<uint32_t> indices;

		VertexBuffer<VertexData> vbvResource_;
		std::vector<VertexData> vertices;




		// トランスフォーム
		std::unique_ptr<Transformation>transformation = nullptr;
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
}
