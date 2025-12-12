#pragma once
#include "BaseSprite.h"
#include "UIData.h"

namespace Engine {
	// 前方宣言
	class Entity2DManager;

	// UI要素基底クラス
	class UIElement
	{
	public:
		virtual ~UIElement() = default;
		// インスタンス設定
		void SetInstance(int instance) { instance_ = instance; }
		// 初期化
		void Init(Entity2DManager* entity2DManager, std::string name);
		// 描画
		void Draw();
		// 更新
		virtual void Update(float deltaTime) = 0;
		// 名前スプライトを使うか設定
		void SetUseNameSprite(bool use) { useNameSprite_ = use; }
		// 位置設定
		void SetPos(const Vector2& pos) { pos_ = pos; }
		// Input設定
		void SetInput(Input* input) { input_ = input; }
		// スプライト追加
		void AddSprite(std::string name, std::string textureName);
		// スプライト取得
		BaseSprite* GetSprite(std::string name);
		// 画面比率設定
		void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
			leftTopPos_ = leftTopPos;
			ratio_ = ratio;
		};
		// 親子付け
		void SetParent(WorldTransform2d* parent) {
			parent_ = parent;
		}

	protected:
		// 固有描画
		virtual void UniqueDraw() = 0;
		// スプライト初期化
		virtual void InitSprite() = 0;
	protected:
		Input* input_;
		std::map<std::string, std::unique_ptr<BaseSprite>> sprites_;
		std::string mainName_;
		Entity2DManager* entity2DManager_ = nullptr;
		Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
		Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率
		bool isDebuck_ = false;	// デバッグか
		int instance_ = 1;		// 生成量
		Vector2 pos_ = {};		// 位置
		bool useNameSprite_ = false;
		WorldTransform2d* parent_;

	};

	// 通常
	class UINormal : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
	protected:
		// 固有描画
		void UniqueDraw()override {};
		//初期化
		void InitSprite()override;
	private:
	};


	// ボタン
	class UIButton : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
	protected:
		// 固有描画
		void UniqueDraw()override {};
		//初期化
		void InitSprite()override;
	private:
		bool isCheck_ = false;
	};

	// チェックボックス
	class UICheckBox : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
		// クリックスプライト取得
		BaseSprite* GetCheckSprite() { return checkSprite.get(); }
		// 背景スプライト取得
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
	protected:
		// 固有描画
		void UniqueDraw()override;
		// 初期化
		void InitSprite()override;
	private:
		std::unique_ptr<BaseSprite> checkSprite;
		std::unique_ptr<BaseSprite> backgroundSprite;
		bool isCheck_ = false;
	};

	// スライダー
	class UISlider : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
		// スライドスプライト取得
		BaseSprite* GetSlidSprite() { return slidSprite.get(); }
		// 背景スプライト取得
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
		// 最小値最大値設定
		void SetMinMax(float min, float max) {
			min_ = min;
			max_ = max;
		};
	protected:
		// 固有描画
		void UniqueDraw()override;
		// 初期化
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
	class UIMeter : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
		// メータスプライト取得
		BaseSprite* GetMeterSprite() { return meterSprite.get(); }
		// 背景スプライト取得
		BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
		// 名前スプライト取得
		BaseSprite* GetNameSprite() { return nameSprite_.get(); }
		// メータ表示タイプ設定
		void SetMeterType(UIMeterType type) { type_ = type; }

		// 背景スプライトのサイズとメーターのサイズ
		void SetMaxSize(const Vector2& size, const Vector2& offsetSise) {
			size_ = size;
			offsetSize = offsetSise;
			backgroundSprite->GetSprite()->SetSize(size_);
			meterSprite->GetSprite()->SetSize(size_);
		}
		// メータ設定
		void SetMeter(float meter) { meter_ = meter; }
		// メータ範囲設定
		void SetMeterMinMax(float min, float max) {
			meterMin_ = min;
			meterMax_ = max;
		};
	protected:
		// 固有描画
		void UniqueDraw()override;
		// 初期化
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
	class UIPair : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
		// 最初スプライト取得
		BaseSprite* GetFirstSprite() { return firstSprite.get(); }
		// 次スプライト取得
		BaseSprite* GetSecondSprite() { return secondSprite.get(); }
		// 間隔設定
		void SetOffset(float offset) { offset_ = offset; }
		// ペアの方向設定
		void SetUIPairDrectionType(UIPairDrectionType type) { type_ = type; }
	protected:
		// 固有描画
		void UniqueDraw()override;
		// 初期化
		void InitSprite()override;
	private:
		float offset_ = 10.0f;
		UIPairDrectionType type_ = UIPairDrectionType::Right;
		std::unique_ptr<BaseSprite> firstSprite;
		std::unique_ptr<BaseSprite> secondSprite;
	};

	// カウンターUI
	class UICount : public UIElement {
	public:
		// 更新
		void Update(float deltaTime)override;
		// カウントのスプライト取得
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

		// 名前スプライト取得
		BaseSprite* GetNameSprite() { return nameSprite_.get(); }

		// 最大カウント設定
		void SetMaxSize(const Vector2& size, const Vector2& offsetSise) {
			size_ = size;
			offsetSize = offsetSise;
			for (auto& count : countSprite_) {
				count->SetSize(size_);
			}
		}
		// テクスチャサイズ設定
		void SetTextuerSize(const Vector2& size) {
			texSize_ = size;
			for (auto& count : countSprite_) {
				count->GetSprite()->SetTextureSize(size);
			}
		}
		// カウント設定
		void SetCount(float count) { count_ = count; }
		// 最大カウント設定
		void SetCountMax(float max) { countMax_ = max; }
	protected:
		// 固有描画
		void UniqueDraw()override;
		// 初期化
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

