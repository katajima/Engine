#pragma once
#include"DirectXGame/engine/2d/Sprite.h"

class Entity2DManager;
class PlayerUI {
public:
	void Initialize(Entity2DManager* entity2DManager);

	void Update();

	void Draw();

	void SetHPBerSize(float size) { sizeHP_ = size; }
	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }

	void SetIsTextmax(bool is) { isTextMax_ = is; }
	void SetIsTextRB(bool is) { isTextRB_ = is; }


private:
	std::unique_ptr<Sprite> HpBer_;
	std::unique_ptr<Sprite> SpecailBer_;
	std::unique_ptr<Sprite> textMax_;
	std::unique_ptr<Sprite> textRB_;
	bool isTextRB_ = false;
	bool isTextMax_ = false;
	float sizeHP_ = 0;
	float sizeSpecialGauge_ = 0;


};

