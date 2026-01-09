#include "BulletEnemyState.h"
#include "BulletEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"

#pragma region Move

// 更新
void BulletEnemyStateMove::Update() {
	BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);

	// HPが0以上なら
	if (character_->GetHP() > 0) {
		// 移動
		character_->Move();
		// 時間更新
		timer_ += character_->GetTime();
	}
	else {
		// 死亡状態に移行
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Die);

	}
};

// 終了
void BulletEnemyStateMove::Exit() {
};

// 初期化
void BulletEnemyStateMove::Enter() {
	timer_ = 0.0f;
	character_->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 3.0f;
};

#pragma endregion // 移動

#pragma region Attack

void BulletEnemyStateAttack::Update() {

	//character_->GetBulletManager()->GenerateBullet();
};

// 終了
void BulletEnemyStateAttack::Exit() {
};
// 初期化
void BulletEnemyStateAttack::Enter() {
};

#pragma endregion // 攻撃

#pragma region Die

// 更新
void BulletEnemyStateDie::Update() {
	// 時間更新
	timer_ -= character_->GetTime();
	if (timer_ <= 0.0f) {
		// 死亡判定に
		character_->SetAlive(false);
		timer_ = 0.0f;
		if (!character_->GetAlive()) {
			character_->Delete();	// キャラクター削除
			character_->GetObjectComponent()->IsDelete();	// オブジェクトコンポーネント削除
		}
	}
	else if (timer_ <= dieTimer_ / 2.0f) {
		character_->GetObjectComponent()->SetIsDraw(false);	// 描画しない	
	}
	else {
		character_->GetObjectComponent()->GetRigidBodyComponent()->SetIsGravity(false);		// 重力無し
		character_->GetObjectComponent()->GetWorldTransform().scale_ -= Vector3(1.1f, 1.1f, 1.1f) * character_->GetTime(); // サイズを縮小
		if (character_->GetObjectComponent()->GetWorldTransform().scale_.x <= 0) {
			character_->GetObjectComponent()->GetWorldTransform().scale_ = Vector3{ 0,0,0 };	// 0に
		}
	}
};

// 終了
void BulletEnemyStateDie::Exit() {
};
// 初期化
void BulletEnemyStateDie::Enter() {
	timer_ = dieTimer_;
};

#pragma endregion // 死亡


