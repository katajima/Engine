#pragma once

#include "DirectXGame/application/base/Attack/Input/AttackInputHandler.h"
#include <functional>
#include <string>

namespace Character {
	class BaseCharacter;
}

namespace GameAction {

	/// <summary>
	/// 受けた攻撃をどの防御状態で処理したかを表します。
	/// </summary>
	enum class DefenseResult {
		Hit,
		Guarded,
		Parried,
		Invincible,
		SuperArmored,
	};

	/// <summary>
	/// キャラクター共通の防御状態を保持します。
	/// </summary>
	struct DefenseState {
		bool guarding = false; // 通常ガード入力を保持しているか
		bool superArmor = false; // 被弾リアクションを抑制するか
		bool invincible = false; // 恒常的なアクション無敵を有効にするか
		bool guardPoint = false; // 攻撃中にもガードを有効にするか
		float parryTimer = 0.0f; // パリィ受付の残り時間
		float actionInvincibleTimer = 0.0f; // 一時無敵の残り時間
		float guardDamageScale = 0.0f; // ガード時に通すダメージ倍率
	};

	/// <summary>
	/// アクション処理のライフサイクルイベント種別です。
	/// </summary>
	enum class EventType {
		AttackStarted,
		HitConfirmed,
		Guarded,
		Parried,
		Missed,
		Canceled,
		ResourceConsumed,
	};

	/// <summary>
	/// アクションイベントの通知データです。
	/// </summary>
	struct Event {
		EventType type = EventType::AttackStarted; // 発生したイベント種別
		Character::BaseCharacter* actor = nullptr; // イベントを発生させたキャラクター
		const Character::BaseCharacter* target = nullptr; // イベント対象のキャラクター
		ActionInput input = ActionInput::LightAttack; // イベントに対応する攻撃入力
		std::string actionName; // コンボノードやアクションの名前
		float value = 0.0f; // リソース量などの数値
	};

	/// <summary>
	/// アクションイベントを受け取るコールバック型です。
	/// </summary>
	using EventListener = std::function<void(const Event&)>;
}
