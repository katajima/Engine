#include "ComboMotion.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"


namespace Combo {

#pragma region ComboMotion

	// 開始
	void ComboMotion::Enter(Character::BaseCharacter* owner) {
		// コンボ用アニメーションクラス
		comboAnimation_.Enter(owner);
		// 移動クラス
		comboMove_.Enter(owner);
	}

	// 更新
	void ComboMotion::Update(const Engine::Input& input, float timer, float dt) {
		// コンボ用アニメーションクラス更新
		comboAnimation_.Update(input, timer, dt);
		// コンボ用移動クラス更新
		comboMove_.Update(input, timer, dt);
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

