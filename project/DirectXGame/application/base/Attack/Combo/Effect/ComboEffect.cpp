#include "ComboEffect.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"


namespace Combo {

#pragma region ComboCamera

	// 開始
	void ComboCamera::Enter() {}

	// 更新
	void ComboCamera::Update(float timer, float dt) {

	}

	// 終了
	void ComboCamera::Exit() {}

#pragma endregion // コンボカメラ


#pragma region ComboEffect

	// 開始
	void ComboEffect::Enter(Character::BaseCharacter* owner) {
		// 武器情報取得
		weapon = owner->GetWeapon();
		// トレイル終了
		weapon->GetObject3D()->isEmitTrailEffect = false;
	}

	// 更新
	void ComboEffect::Update(float timer, float dt) {
		// トレイル使用可能か
		bool isTrail = false;
		if (IsEffectTrail(timer)) { isTrail = true; }
		// トレイルを出すか設定
		weapon->GetObject3D()->isEmitTrailEffect = isTrail;
	}

	// 終了
	void ComboEffect::Exit(Character::BaseCharacter* owner) {
		// トレイル終了
		weapon->GetObject3D()->isEmitTrailEffect = false;
	}

#pragma endregion //コンボエフェクト

}