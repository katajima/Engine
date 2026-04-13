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
		// データ
		struct Data {
			// 発射スタート
			float rangeWindowStart = 0.1f;		
			// 発射エンド
			float rangeWindowEnd = 0.5f;			
			// 弾速
			float speed = 1.0f;
			// 弾の発射間隔
			float intarval = 0.5f;
			// 発射する弾の数
			int count = 1;
			//　ダメージ
			float damage = 1.0f;
		};
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
	};

}
