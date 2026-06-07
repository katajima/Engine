#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include "DirectXGame/application/base/Attack/Combo/Range/ComboBullet.h"
#include "DirectXGame/application/base/Attack/Combo/Range/ComboThrow.h"
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

// 前方宣言
namespace Character {
	class BaseCharacter;		// キャラクター
}

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
		// 現在のコンボ攻撃がヒットしたことを通知する
		void NotifyHit();
		// データ構造体取得
		Data& GetData() { return data_; }
	private:
		// 遠距離攻撃の狙い方向を取得する
		Vector3 ResolveAimDirection(const Character::CharacterContext& ctx) const;
		// 遠距離攻撃の狙い位置を取得する
		Vector3 ResolveAimTarget(const Character::CharacterContext& ctx) const;
		// オフセットターゲットの狙い位置を取得する
		Vector3 ResolveOffsetAimTarget(const Character::CharacterContext& ctx) const;

		// コンボ使用者
		Character::BaseCharacter* owner_ = nullptr;
		// データ
		Data data_;
		ComboBullet bullet_;	// 弾を使用する遠距離処理
		ComboThrow throw_;		// 投擲物を使用する遠距離処理
	};

}
