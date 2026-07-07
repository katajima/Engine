#include "ComboMotion.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"


namespace Combo {

#pragma region ComboMotion

	// 開始
	void ComboMotion::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// コンボ用アニメーションクラス
		comboAnimation_.SetEndConditionType(endType);
		comboAnimation_.Enter(owner);
		// 移動クラス
		comboMove_.Enter(owner, ctx);
	}

	// 更新
	void ComboMotion::Update(const Character::CharacterContext& ctx, float timer,bool isDebug) {
		// コンボ用アニメーションクラス更新
		comboAnimation_.Update(ctx,timer,isDebug);
		// コンボ用移動クラス更新
		comboMove_.Update(ctx, timer);
	}

	// 終了
	void ComboMotion::Exit(Character::BaseCharacter* owner) {
		// アニメーションクラス
		comboAnimation_.Exit(owner);
		// 移動クラス
		comboMove_.Exit(owner);
	}

#pragma endregion // コンボモーション
};

