#include "AttackData.h"

void ComboCondition::Update(float dt) {

	timer_ += dt;


	bool isStart	= inputWindowStart_ <= timer_;	// 受付開始時間を過ぎたら
	bool isEnd = inputWindowEnd_ >= timer_;			// 受付終了時間より前なら



	// 受付時間内なら
	if (isStart && isEnd) {

	}
}


void ComboCondition::Enter() {
	timer_ = 0.0f;
	isNextCombo_ = false;
};

void ComboCondition::Exit() {
	timer_ = 0.0f;
	isNextCombo_ = false;
};