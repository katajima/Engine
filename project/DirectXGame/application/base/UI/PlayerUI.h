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
};

