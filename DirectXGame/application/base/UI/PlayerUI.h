#pragma once
#include "DirectXGame/application/base/BaseClass/UI/BaseUI.h"

class Entity2DManager;
class PlayerUI : public BaseUI{
public:
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	void Update() override;


	void Draw() override;




public:
	void SetHPBerSize(float size) { sizeHP_ = size; }
	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }
	void SetIsTextmax(bool is) { isTextMax_ = is; }
	void SetIsTextRB(bool is) { isTextRB_ = is; }
private:

	float srideM_ = 0;

	std::unique_ptr<Sprite> textMax_;
	std::unique_ptr<Sprite> textRB_;
	bool isTextRB_ = false;
	bool isTextMax_ = false;
	float sizeHP_ = 0;
	float sizeSpecialGauge_ = 0;
};

