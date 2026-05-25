#include "ComboCondition.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

#pragma region ComboCondition

void Combo::ComboCondition::Update(const Character::CharacterContext& ctx, float timer) {

	// キャンセル受付クラス更新
	cancelReceiver_.Update(ctx,data_, timer);
	// 終了条件
	endCondition_.Update(ctx, data_, timer);
	// 移行条件
	nextCondition_.Update(ctx, data_, timer);
}

void Combo::ComboCondition::Enter(Character::BaseCharacter* owner) {
	// キャンセル受付クラス
	cancelReceiver_.Enter();
	// 移行受付クラス
	nextRecever_.Enter(data_);
	// 終了条件クラス
	endCondition_.Enter(data_);
	// 移行条件クラス
	nextCondition_.Enter(data_);
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
