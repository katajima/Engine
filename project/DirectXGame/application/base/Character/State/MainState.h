#pragma once

namespace Character {
	// キャラクター行動(一段階)
	enum class CharacterMainState {
		Idle,		// 待機
		Move,       // 移動
		Jump,       // ジャンプ
		Avoidance,  // 回避
		Defense,    // 防御
		Attack,     // 攻撃
		Skill,      // スキル
		Special,    // 必殺技
		Die,        // 死亡
		Fainting,   // 気絶
		Damage,     // 被弾
		Dash,       // ダッシュ
	};
}