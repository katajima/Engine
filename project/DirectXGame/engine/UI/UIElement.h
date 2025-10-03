#pragma once
#include "BaseSprite.h"
#include "UIData.h"



class Entity2DManager;
class UIElement
{
public:
	virtual ~UIElement() = default;  

	void SetInstance(int instance) {instance_ = instance;}
	void Init(Entity2DManager* entity2DManager, std::string name);
	void Draw();
	virtual void Update(float deltaTime) = 0;
	void SetUseNameSprite(bool use) { useNameSprite_ = use; }
	void SetPos(const Vector2& pos) { pos_ = pos; }
	void SetInput(Input* input) { input_ = input; }
	void AddSprite(std::string name, std::string textureName);

	BaseSprite* GetSprite(std::string name);

	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;
		ratio_ = ratio;
	};

	void SetParent(WorldTransform2d* parent) {
		parent_ = parent;
	}

protected:
	virtual void UniqueDraw() = 0;
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
	void Update(float deltaTime)override;
protected:
	void UniqueDraw()override {};
	void InitSprite()override;
private:
};


// ボタン
class UIButton : public UIElement {
public:
	void Update(float deltaTime)override;
protected:
	void UniqueDraw()override {};
	void InitSprite()override;
private:
	bool isCheck_ = false;
};

// チェックボックス
class UICheckBox : public UIElement {
public:
	void Update(float deltaTime)override;
	BaseSprite* GetCheckSprite() { return checkSprite.get(); }
	BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
protected:
	void UniqueDraw()override;
	void InitSprite()override;
private:
	std::unique_ptr<BaseSprite> checkSprite;
	std::unique_ptr<BaseSprite> backgroundSprite;
	bool isCheck_ = false;
};

// スライダー
class UISlider : public UIElement {
public:
	void Update(float deltaTime)override;
	BaseSprite* GetSlidSprite() { return slidSprite.get(); }
	BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
	void SetMinMax(float min, float max) {
		min_ = min;
		max_ = max;
	};
protected:
	void UniqueDraw()override;
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
	void Update(float deltaTime)override;
	BaseSprite* GetMeterSprite() { return meterSprite.get(); }
	BaseSprite* GetBackgroundSprite() { return backgroundSprite.get(); }
	BaseSprite* GetNameSprite() { return nameSprite_.get(); }
	void SetMeterType(UIMeterType type) { type_ = type; }

	// 背景スプライトのサイズとメーターのサイズ
	void SetMaxSize(const Vector2& size,const Vector2& offsetSise) {
		size_ = size;
		offsetSize = offsetSise;
		backgroundSprite->GetSprite()->SetSize(size_);
		meterSprite->GetSprite()->SetSize(size_);
	}

	void SetMeter(float meter) { meter_ = meter; }

	void SetMeterMinMax(float min, float max) {
		meterMin_ = min;
		meterMax_ = max;
	};
protected:
	void UniqueDraw()override;
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
	void Update(float deltaTime)override;
	BaseSprite* GetFirstSprite() { return firstSprite.get(); }
	BaseSprite* GetSecondSprite() { return secondSprite.get(); }
	
	void SetOffset(float offset) {offset_ = offset;}
	void SetUIPairDrectionType(UIPairDrectionType type) { type_ = type; }
protected:
	void UniqueDraw()override;
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
	void Update(float deltaTime)override;
	BaseSprite* GetCountSprite(int num) { 
		if (static_cast<int>(countSprite_.size()) <= num) {
			num = static_cast<int>(countSprite_.size()) -1;
		}
		return countSprite_[num].get(); 
	}
	BaseSprite* GetNameSprite() { return nameSprite_.get(); }
	
	// 
	void SetMaxSize(const Vector2& size, const Vector2& offsetSise) {
		size_ = size;
		offsetSize = offsetSise;
		for (auto& count : countSprite_) {
			count->SetSize(size_);
		}
	}

	void SetTextuerSize(const Vector2& size) {
		texSize_ = size;
		for (auto& count : countSprite_) {
			count->GetSprite()->SetTextureSize(size);
		}
	}

	void SetCount(float count) { count_ = count; }

	void SetCountMax(float max) { countMax_ = max;}
protected:
	void UniqueDraw()override;
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


