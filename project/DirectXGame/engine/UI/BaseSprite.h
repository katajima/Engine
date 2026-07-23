#pragma once
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/Color/ColorComponent.h"


namespace Engine {
	enum class SpriteSize {
		Top,
		Down,
		Right,
		Left,
	};

	// 前方宣言
	class EntityManager;

	// スプライト基底クラス
/// <summary>
/// BaseSpriteを管理・実装するクラス。
/// </summary>
	class BaseSprite {
	public:
		// 初期化
		void Init(EntityManager* entityManager, std::string name, std::string texturName);
		// 更新
		void Update() {
			// コライダーあるか
			if (useColl) {
				BoxUpdate();
			}
			// 更新
			sprite_->Update();
		};
		// 描画
		void Draw() {
			// スプライトが生きているなら描画
			if (sprite_->GetActive()) {
				sprite_->Draw();
			}
		}

		// Box取得
		Box GetBox() const { return box; }
		// スプライト取得
		Sprite* GetSprite() { return sprite_.get(); }
		// サイズ取得
		Vector2 GetSize() { return sprite_->GetSize(); }
		// 位置取得
		Vector2 GetPos() { return sprite_->GetPosition(); }
		// アンカーポイント取得
		Vector2 GetAnchorPoint() { return sprite_->GetAnchorPoint(); }
		// 色コンポーネント取得
		ColorComponent* GetColorComponent() { return colorComponent_.get(); }

		// サイズ設定
		void SetSize(const Vector2& size) { sprite_->SetSize(size); }
		// 位置設定
		void SetPos(const Vector2& pos) { sprite_->SetPosition(pos); }
		// アンカーポイント設定
		void SetAnchorPoint(const Vector2& anchor) { sprite_->SetAnchorPoint(anchor); }
		// テクスチャ名設定
		void SetTextureName(const std::string& name) { sprite_->SetTexture(name); };
		// 色設定
		void SetColor(Color color) { sprite_->SetColor(color); }
		// コライダ判定を取るか設定
		void SetUseColl(bool is) { useColl = is; }
		// 比率設定
		void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
			leftTopPos_ = leftTopPos;
			ratio_ = ratio;
		};
		// 比率
		Vector2 GetRatio() const { return ratio_; }
		// 半分のサイズ取得
		Vector2 GetHalfSise(SpriteSize type) {
			Vector2 size = sprite_->GetSize();
			Vector2 anchor = sprite_->GetAnchorPoint();
			Vector2 result{};

			switch (type)
			{
			case SpriteSize::Top:
				result = { 0.0f, size.y * anchor.y };
				break;
			case SpriteSize::Down:
				result = { 0.0f, size.y * (1.0f - anchor.y) };
				break;
			case SpriteSize::Left:
				result = { size.x * anchor.x, 0.0f };
				break;
			case SpriteSize::Right:
				result = { size.x * (1.0f - anchor.x), 0.0f };
				break;
			default:
				result = { 0.0f, 0.0f };
				break;
			}

			return result;
		}
	private:
		// Box更新
		void BoxUpdate() {
			Vector2 position = sprite_->GetWorldTransform2d().GetWorldPosition();
			Vector2 size = sprite_->GetSize();
			Vector2 anchor = sprite_->GetAnchorPoint();

			// スプライトのローカルでの AABB
			Vector2 localMin = position - size * anchor;
			Vector2 localMax = position + size * (Vector2(1.0f, 1.0f) - anchor);


			box.min_ = localMin;
			box.max_ = localMax;
			// ImGuiImage 上でのワールド座標に変換（左上位置 + 比率）
#ifdef _DEBUG
			box.min_ = leftTopPos_ + localMin * ratio_;
			box.max_ = leftTopPos_ + localMax * ratio_;
#endif // _DEBUG
		}
	private:
		std::unique_ptr<Sprite> sprite_;
		std::unique_ptr <ColorComponent> colorComponent_ = nullptr;
		Box box;
		bool useColl = false;

		Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
		Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率
	};
}
