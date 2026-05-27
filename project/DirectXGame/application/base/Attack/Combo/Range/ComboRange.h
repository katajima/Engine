#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

// 前方宣言
namespace Character {
	class BaseCharacter;		// キャラクター
}
// 弾出現
class BulletSpawn;

namespace Combo {

	/// <summary>
	/// 遠距離用コンボクラス
	/// </summary>
	class ComboRange {
	public:
		using Data = GlobalRange;
		// 開始
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);
		// 更新
		void Update(const Character::CharacterContext& ctx, float timer);
		// 終了
		void Exit(Character::BaseCharacter* owner);
		// データ構造体取得
		Data& GetData() { return data_; }
	private:
		// 弾の出現処理クラス
		BulletSpawn* bulletSpawn = nullptr;	
		// データ
		Data data_;
		// 発射した弾の数
		int bulletCount = 0;
		float nextShotTime = 0.0f;
	};

}
