#pragma once
#include<memory>

// 攻撃種類
enum class AttackInput {
	Light,          // 弱攻撃
	Heavy,          // 強攻撃
	Special,        // 必殺技など特殊攻撃
	Skill,          // スキル攻撃
	JumpAttack,     // 空中攻撃
	DashAttack,     // ダッシュ中に攻撃
	Charge,         // チャージ攻撃（ボタン長押し）
	Cancel,         // コンボキャンセル（ステップや回避などに移行）
	Parry,          // 敵の攻撃に対するカウンター
};

// 前方宣言
namespace Character {
	class BaseCharacter;
}
