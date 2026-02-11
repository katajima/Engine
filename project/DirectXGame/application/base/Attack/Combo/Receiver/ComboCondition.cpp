#include "ComboCondition.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

#pragma region ComboCondition

void Combo::ComboCondition::Update(const Engine::Input& input, float timer, float dt) {

	// キャンセル受付クラス更新
	if (!nextRecever_.GetIsNext()) {
		cancelReceiver_.Update(input, timer);
	}
	// コンボ入力受付クラス更新
	if (!cancelReceiver_.GetIsCancel()) {
		nextRecever_.Update(input, timer);
	}
	// 終了条件
	endCondition_.Update(input, jumpSystem, timer, dt);
	// 移行条件
	nextCondition_.Update(input, jumpSystem, timer, dt);
}

void Combo::ComboCondition::Enter(Character::BaseCharacter* owner) {
	jumpSystem = owner->GetMoveComponent()->GetJumpSystem();	// ジャンプシステム取得
	// キャンセル受付クラス
	cancelReceiver_.Enter();
	// 移行受付クラス
	nextRecever_.Enter();
	// 終了条件クラス
	endCondition_.Enter();
	// 移行条件クラス
	nextCondition_.Enter();
};

void Combo::ComboCondition::Exit() {
	// キャンセル受付クラス
	cancelReceiver_.Exit();
	// 移行受付クラス
	nextRecever_.Exit();
	// 終了条件クラス
	endCondition_.Exit();
	// 移行条件クラス
	nextCondition_.Exit();
}
#pragma endregion // コンボ条件
