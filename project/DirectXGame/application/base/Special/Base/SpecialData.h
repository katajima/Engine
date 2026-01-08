#pragma once
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"




struct SpecialData{
	// ゲージ
	int gauge_ = 0;
	// 最大値
	int maxGauge_ = 1;
	// スペシャル攻撃発動可能か
	bool isUse_ = false;
	// スペシャル攻撃発動中か
	bool isAction_ = false;
	// フェーズ
	int phase_ = 0;
	// 移動可能か
	bool isMove_ = true;

	// 最大値まで行ったか
	bool IsMax() const { return gauge_ >= maxGauge_; }
	// フェーズ移行
	void AddPhase() { phase_++;}
};