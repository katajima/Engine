#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"

namespace Character {
	class BaseCharacter;
	struct CharacterContext;
}

class BulletSpawn;

namespace Combo {

	/// <summary>
	/// コンボ遠距離の弾生成処理
	/// </summary>
	class ComboBullet {
	public:
		/// <summary>
		/// 弾生成処理の開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner, const GlobalRange& data);

		/// <summary>
		/// 弾生成処理の更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx, float timer, const GlobalRange& data, const Vector3& aimTarget);

		/// <summary>
		/// 弾生成処理の終了
		/// </summary>
		void Exit();

	private:
		Character::BaseCharacter* owner_ = nullptr;	// コンボ使用者
		BulletSpawn* bulletSpawn_ = nullptr;			// 弾生成クラス
		int bulletCount_ = 0;						// 発射済みの弾数
		float nextShotTime_ = 0.0f;					// 次の発射時間
	};
}
