#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
class Entity2DManager;

/// <summary>
/// プレイヤーUIクラス
/// </summary>
class PlayerUI : public BaseUI{
public:

	// 初期化
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;




public:

	// HP設定
	void SetHP(Gage* hp) { hpBar_ = hp; };
	// スタミナ設定
	void SetStamina(Gage* stamina) { staminaBar_ = stamina; }

	// スペシャルゲージサイズ設定
	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }
	// maxテクスチャ映すか
	void SetIsTextmax(bool is) { isTextMax_ = is; }
	// RBボタンテクスチャ映すか
	void SetIsTextRB(bool is) { isTextRB_ = is; }
private:

	Gage* hpBar_;
	Gage* staminaBar_;



	float srideM_ = 0;

	std::unique_ptr<Sprite> textMax_;
	std::unique_ptr<Sprite> textRB_;
	bool isTextRB_ = false;
	bool isTextMax_ = false;
	float sizeSpecialGauge_ = 0;



	struct HpSpriteData{
		Vector2 pos_ = { 30,600 };

		Color color_ = { 0,1,0,1 };
		Color nameColor_ = { 0.75f,0.75f ,0.75f ,1.0f };

		float maxMeter = 100.0f;
		Vector2 offset_ = { 4.0f,4.0f };

		Vector2 size_ = { 200.0f,30.0f };
		Vector2 nameSize_ = { 60.0f,40.0f };

	};
	HpSpriteData hpSpriteData_;
	
	struct SpSpriteData {
		Vector2 pos_ = { 30,650 };

		Color color_ = { 0,0,1,1 };
		Color nameColor_ = { 0.75f,0.75f ,0.75f ,1.0f };

		float maxMeter = 100.0f;
		Vector2 offset_ = { 4.0f,4.0f };

		Vector2 size_ = { 200.0f,30.0f };
		Vector2 nameSize_ = { 60.0f,40.0f };

	};
	SpSpriteData spSpriteData_;

	struct StaminaSpriteData {
		Vector2 pos_ = { 30,700 };

		Color color_ = { 1,1,0,1 };
		Color nameColor_ = { 0.75f,0.75f ,0.75f ,1.0f };

		float maxMeter = 20.0f;
		Vector2 offset_ = { 4.0f,4.0f };

		Vector2 size_ = { 200.0f,30.0f };
		Vector2 nameSize_ = { 60.0f,40.0f };

	};
	StaminaSpriteData staminaSpriteData;

	struct MaxTextData {
		float size_ = 0.25f;
		Vector2 pos_ = { 30,700 };
		float rotate_ = Math::DegreesToRadians(-30);
		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Color color_ = { 1,0,0,1 };
	};
	MaxTextData maxTextData_;

	struct RBData {
		float size_ = 0.2f;
		Vector2 pos_ = { 1280 / 2,550 };
		float rotate_ = Math::DegreesToRadians(-30);
		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Color color_ = { 1,1,1,1 };
	};
	RBData rbData_;

};

