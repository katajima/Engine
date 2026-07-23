#pragma once
#include "BaseSprite.h"
#include "UIData.h"

class InputSystem;

namespace Engine {
	// 前方宣言
	class EntityManager;

	// UI要素基底クラス
/// <summary>
/// UIElementを管理・実装するクラス。
/// </summary>
	class UIElement
	{
	public:
		virtual ~UIElement() = default;
		/// <summary>
		/// インスタンス設定
		/// </summary>
		void SetInstance(int instance) { instance_ = instance; }
		/// <summary>
		/// 初期化
		/// </summary>
		void Init(EntityManager* entityManager, std::string name);
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update(float deltaTime) = 0;
		/// <summary>
		/// 名前スプライトを使うか設定
		/// </summary>
		void SetUseNameSprite(bool use) { useNameSprite_ = use; }
		/// <summary>
		/// 位置設定
		/// </summary>
		void SetPos(const Vector2& pos) { pos_ = pos; }
		/// <summary>
		/// Input設定
		/// </summary>
		void SetInput(InputSystem* inputSystem) { this->inputSystem = inputSystem; }
		/// <summary>
		/// スプライト追加
		/// </summary>
		void AddSprite(std::string name, std::string textureName);
		/// <summary>
		/// スプライト取得
		/// </summary>
		BaseSprite* GetSprite(std::string name);
		/// <summary>
		/// 画面比率設定
		/// </summary>
		void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
			leftTopPos_ = leftTopPos;
			ratio_ = ratio;
		};
		/// <summary>
		/// 親子付け
		/// </summary>
		void SetParent(WorldTransform2d* parent) {
			this->parent = parent;
		}

	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		virtual void UniqueDraw() = 0;
		/// <summary>
		/// スプライト初期化
		/// </summary>
		virtual void InitSprite() = 0;
	protected:
		InputSystem* inputSystem = nullptr;
		EntityManager* entityManager = nullptr;
		WorldTransform2d* parent = nullptr;
	protected:
		std::map<std::string, std::unique_ptr<BaseSprite>> sprites_;
		std::string mainName_;
		Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
		Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率
		bool isDebug_ = false;	// デバッグか
		int instance_ = 1;		// 生成量
		Vector2 pos_ = {};		// 位置
		bool useNameSprite_ = false;

	};

	// 通常
/// <summary>
/// UINormalを管理・実装するクラス。
/// </summary>
	class UINormal : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override {};
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
	};


	// ボタン
/// <summary>
/// UIButtonを管理・実装するクラス。
/// </summary>
	class UIButton : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override {};
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		bool isCheck_ = false;
	};

	// チェックボックス
/// <summary>
/// UICheckBoxを管理・実装するクラス。
/// </summary>
	class UICheckBox : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
		/// <summary>
		/// クリックスプライト取得
		/// </summary>
		BaseSprite* GetCheckSprite() { return checkSprite.get(); }
		/// <summary>
		/// 背景スプライト取得
		/// </summary>
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override;
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		std::unique_ptr<BaseSprite> checkSprite;
		std::unique_ptr<BaseSprite> backgroundSprite;
		bool isCheck_ = false;
	};

	// スライダー
/// <summary>
/// UISliderを管理・実装するクラス。
/// </summary>
	class UISlider : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
		/// <summary>
		/// スライドスプライト取得
		/// </summary>
		BaseSprite* GetSlidSprite() { return slidSprite.get(); }
		/// <summary>
		/// 背景スプライト取得
		/// </summary>
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
		/// <summary>
		/// 最小値最大値設定
		/// </summary>
		void SetMinMax(float min, float max) {
			min_ = min;
			max_ = max;
		};
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override;
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		std::unique_ptr<BaseSprite> slidSprite;
		std::unique_ptr<BaseSprite> backgroundSprite;
		float min_;
		float max_;
		bool isClick = false;

		Vector2 preMousePos{};
		Vector2 offsetPos_{};
		// クラスメンバに追加
		float dragOffsetX_ = 0.0f;
	};


	// メーター
/// <summary>
/// UIMeterを管理・実装するクラス。
/// </summary>
	class UIMeter : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
		/// <summary>
		/// メータスプライト取得
		/// </summary>
		BaseSprite* GetMeterSprite() { return meterSprite.get(); }
		/// <summary>
		/// 背景スプライト取得
		/// </summary>
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
		/// <summary>
		/// 名前スプライト取得
		/// </summary>
		BaseSprite* GetNameSprite() { return nameSprite_.get(); }
		/// <summary>
		/// メータ表示タイプ設定
		/// </summary>
		void SetMeterType(UIMeterType type) { type_ = type; }

		/// <summary>
		/// 背景スプライトのサイズとメーターのサイズ
		/// </summary>
		void SetMaxSize(const Vector2& size, const Vector2& offsetSise) {
			size_ = size;
			offsetSize = offsetSise;
			backgroundSprite->GetSprite()->SetSize(size_);
			meterSprite->GetSprite()->SetSize(size_);
		}
		/// <summary>
		/// メータ設定
		/// </summary>
		void SetMeter(float meter) { meter_ = meter; }
		/// <summary>
		/// メータ範囲設定
		/// </summary>
		void SetMeterMinMax(float min, float max) {
			meterMin_ = min;
			meterMax_ = max;
		};
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override;
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		Vector2 size_ = {};
		Vector2 offsetSize = {};
		//Vector2
		std::unique_ptr<BaseSprite> meterSprite;
		std::unique_ptr<BaseSprite> backgroundSprite;
		std::unique_ptr<BaseSprite> nameSprite_;
		UIMeterType type_ = UIMeterType::Left;

		float meter_;									// 現在のメータ
		float meterMin_;								// メーター下限
		float meterMax_;								// メーター上限
	};

	//  ペア
/// <summary>
/// UIPairを管理・実装するクラス。
/// </summary>
	class UIPair : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
		/// <summary>
		/// 最初スプライト取得
		/// </summary>
		BaseSprite* GetFirstSprite() { return firstSprite.get(); }
		/// <summary>
		/// 次スプライト取得
		/// </summary>
		BaseSprite* GetSecondSprite() { return secondSprite.get(); }
		/// <summary>
		/// 間隔設定
		/// </summary>
		void SetOffset(float offset) { offset_ = offset; }
		/// <summary>
		/// ペアの方向設定
		/// </summary>
		void SetUIPairDirectionType(UIPairDirectionType type) { type_ = type; }
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override;
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		float offset_ = 10.0f;
		UIPairDirectionType type_ = UIPairDirectionType::Right;
		std::unique_ptr<BaseSprite> firstSprite;
		std::unique_ptr<BaseSprite> secondSprite;
	};

	// カウンターUI
/// <summary>
/// UICountを管理・実装するクラス。
/// </summary>
	class UICount : public UIElement {
	public:
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime)override;
		/// <summary>
		/// カウントのスプライト取得
		/// </summary>
		BaseSprite* GetCountSprite(int num) {
			if (static_cast<int>(countSprite_.size()) <= num) {
				num = static_cast<int>(countSprite_.size()) - 1;
			}
			return countSprite_[num].get();
		}


		void SetCountColor(const Color& color) {
			for (auto& count : countSprite_) {
				count->GetSprite()->SetColor(color);
			}
		}

		/// <summary>
		/// 名前スプライト取得
		/// </summary>
		BaseSprite* GetNameSprite() { return nameSprite_.get(); }

		/// <summary>
		/// 最大カウント設定
		/// </summary>
		void SetMaxSize(const Vector2& size, const Vector2& offsetSise) {
			size_ = size;
			offsetSize = offsetSise;
			for (auto& count : countSprite_) {
				count->SetSize(size_);
			}
		}
		/// <summary>
		/// テクスチャサイズ設定
		/// </summary>
		void SetTextureSize(const Vector2& size) {
			texSize_ = size;
			for (auto& count : countSprite_) {
				count->GetSprite()->SetTextureSize(size);
			}
		}
		/// <summary>
		/// カウント設定
		/// </summary>
		void SetCount(float count) { count_ = count; }
		/// <summary>
		/// 最大カウント設定
		/// </summary>
		void SetCountMax(float max) { countMax_ = max; }
	protected:
		/// <summary>
		/// 固有描画
		/// </summary>
		void UniqueDraw()override;
		/// <summary>
		/// 初期化
		/// </summary>
		void InitSprite()override;
	private:
		Vector2 size_{};
		Vector2 offsetSize{};
		Vector2 texSize_{};
		std::vector<std::unique_ptr<BaseSprite>> countSprite_;
		std::unique_ptr<BaseSprite> nameSprite_;

		float count_;									// 現在のメータ
		float countMax_;								// メーター上限
	};
}

