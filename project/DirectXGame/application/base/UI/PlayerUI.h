#pragma once
#include "DirectXGame/application/base/BaseClass/UI/BaseUI.h"

class Entity2DManager;
class PlayerUI : public BaseUI{
public:
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	void Update() override;


	void Draw() override;




public:


	void SetHP(Gage* hp) { hpBar_ = hp; };
	void SetStamina(Gage* stamina) { staminaBar_ = stamina; }


	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }
	void SetIsTextmax(bool is) { isTextMax_ = is; }
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

