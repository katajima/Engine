#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"
#include "DirectXGame/application/base/Weapon/Player/PlayerSubWeapon.h"

namespace Character {
	class BaseCharacter;
}

namespace Combo {

	/// <summary>
	/// コンボ遠距離の投擲物処理
	/// </summary>
	class ComboThrow {
	public:
		/// <summary>
		/// 投擲処理の開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner, const GlobalRange& data);

		/// <summary>
		/// 投擲処理の更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx, float timer, const GlobalRange& data,
			const Vector3& aimDirection, const Vector3& aimTarget);

		/// <summary>
		/// 投擲処理の終了
		/// </summary>
		void Exit(const GlobalRange& data);

		/// <summary>
		/// 現在のコンボ攻撃がヒットしたことを通知する
		/// </summary>
		void NotifyHit(const GlobalRange& data);

	private:
		/// <summary>
		/// 投擲物を生成する
		/// </summary>
		void UpdateThrowSpawn(const Character::CharacterContext& ctx, float timer, const GlobalRange& data,
			const Vector3& aimDirection, const Vector3& aimTarget);

		/// <summary>
		/// 効果発動条件を更新する
		/// </summary>
		void UpdateEffectTrigger(const Character::CharacterContext& ctx, float timer, const GlobalRange& data);

		/// <summary>
		/// 回収条件を更新する
		/// </summary>
		void UpdateRecallTrigger(const Character::CharacterContext& ctx, float timer, const GlobalRange& data);

		/// <summary>
		/// 回収前の停滞時間を更新する
		/// </summary>
		void UpdateRecallStay(float dt, const GlobalRange& data, PlayerSubWeapon* subWeapon);

		/// <summary>
		/// サブ武器の移動タイプへ変換する
		/// </summary>
		PlayerSubWeaponThrowData::MoveType ToSubWeaponMoveType(const GlobalRange& data) const;

		/// <summary>
		/// プレイヤーのサブ武器を取得する
		/// </summary>
		PlayerSubWeapon* GetPlayerSubWeapon() const;

		Character::BaseCharacter* owner_ = nullptr;	// コンボ使用者
		int throwCount_ = 0;						// 投擲済み数
		float nextThrowTime_ = 0.0f;				// 次の投擲時間
		bool isEffectActive_ = false;				// 効果が発動済みか
		bool isHit_ = false;						// 現在のコンボでヒットしたか
		bool isRecallWaiting_ = false;				// 回収前の停滞中か
		float stayTimer_ = 0.0f;					// 回収前の停滞時間
	};
}
