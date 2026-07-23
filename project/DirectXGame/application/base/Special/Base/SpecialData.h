#pragma once
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


namespace Special {
	enum class Type {
		Melee,	// 近距離
		Ranged,	// 遠距離
		Mix,	// 両方
	};

	// 保存用データ
	struct GlobalData {
		// 発動条件
		int maxGauge_ = 0;
		// アニメーション
		std::string animationName_ = "";
		// アニメーション速度
		float animationSpeed = 1.0f;
		// 技の攻撃方法
		Type type;
	};
};


// 必殺技データ
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
	// 時間を止めるか
	bool isStopTimer_ = false;
	/// <summary>
	/// 最大値まで行ったか
	/// </summary>
	bool IsMax() const { return gauge_ >= maxGauge_; }
	/// <summary>
	/// フェーズ移行
	/// </summary>
	void AddPhase() { phase_++;}
};

