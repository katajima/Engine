#include "NormalPlayerState.h"
#include "NormalPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>

#pragma region Idle

// 更新
void PlayerStateIdle::Update() {
	Input* input = character_->GetInput();
	BaseWeapon* weapon = character_->GetWeapon();
	BaseSpecial* special = character_->GetSpecial();
	
	// 武器描画 
	weapon->GetObject3D()->SetIsDraw(true);

	// ゲームパッドが繋いであるなら
	if (input->IsControllerConnected()) {

		// 必殺技が使えるようになったら
		if (character_->GetSpecial()->GetIsSpecial()) {
			special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		}

		// スキル発動
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
			return;
		}

		// 防御
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Defense);
			return;
		}

	}

#ifdef _DEBUG

	// デバッグ用
	if (input->IsTriggerKey(DIK_Z)) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		return;
	}
#endif // _DEBUG

	weapon->RecastTime(MyGame::GameTime());

	// 必殺技移行
	if (special->GetIsSpecial()) {
		if (special->GetIsSpecialAttack()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
			return;
		}
	}

	// 移動したら
	if (input->GetGamePadLeftStick().Length() != 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}
};

// 終了
void PlayerStateIdle::Exit() {
};

// 初期化
void PlayerStateIdle::Enter() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsLoop(true);			// ループさせる
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetAnimationSpeed(1.0f);	//　アニメーションスピード設定
	anima->SetAnimetion("Idle1", 0.1f);	// 流すアニメーション設定
};

#pragma endregion // 待機

#pragma region Move

void PlayerStateMove::Update()
{
	Input* input = character_->GetInput();				// 入力
	BaseWeapon* weapon = character_->GetWeapon();		// 武器
	BaseSpecial* special = character_->GetSpecial();	// 必殺

	weapon->GetObject3D()->SetIsDraw(true); // 武器描画

	// ゲームパッドがつながっているなら
	if (input->IsControllerConnected()) {
		// スペシャル発動
		if (character_->GetSpecial()->GetIsSpecial()) {
			special->SetIsSpecialAttack(input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB));
		}
		// スキル発動
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
			return;
		}
		// 防御発動
		if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_A)) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Defense);
			return;
		}

	}

	// 気絶発動
	if (input->IsTriggerKey(DIK_Z)) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		return;
	}

	// 武器リキャストタイム更新
	weapon->RecastTime(MyGame::GameTime());
	
	// 必殺がうてるなら
	if (special->GetIsSpecial()) {
		if (special->GetIsSpecialAttack()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
			return;
		}
	}

	// 止まったら
	if (input->GetGamePadLeftStick().Length() == 0) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
		return;
	}
}

void PlayerStateMove::Exit()
{

}

void PlayerStateMove::Enter()
{
	BaseWeapon* weapon = character_->GetWeapon();
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	weapon->GetObject3D()->SetIsDraw(false);	// 武器描画しない
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false);	// 武器コライダー無効
	anima->SetIsLoop(true);				// ループ再生
	anima->SetIsPlaying(true);			// 再生
	anima->SetAnimationSpeed(1.0f);		// アニメーションスピード設定
	anima->SetAnimetion("Walk", 0.1f);	// 流すアニメーション設定
}

#pragma endregion // 移動

#pragma region Jump

// 更新
void PlayerStateJump::Update() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	Input* input = character_->GetInput();

	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定


	// ジャンプ出来るか
	bool isJamp = character_->GetMoveComponent()->GetIsJump();
	bool isTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_Y);
	bool isAlive = character_->GetAlive();


	// キャラクターが生きていてジャンプ回数が残っていて着地状態じゃないのなら
	if (isAlive && isJamp && isTrigger) {

		character_->GetObjectComponent()->GetRigidBodyComponent()->Velocity().y = 0;
		character_->GetMoveComponent()->DecrementJumpCount(); // ジャンプ回数減少
		
		// 着地状態なら
		if (character_->GetMoveComponent()->GetIsLanding()) {

		}
		else {
			character_->GetObjectComponent()->GetRigidBodyComponent()->AddForce({ 0,character_->GetCharacterParameterComponent().parameters_.jampPower * 2,0 });
		}
		character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimetion("JumpStrat1", 0.05f);
	}
	else {
		// 着地状態なら
		if (character_->GetMoveComponent()->GetIsLanding()) {
			character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}
	}


	// 降下しているならアニメーションを変える
	if (character_->GetObjectComponent()->GetRigidBodyComponent()->Velocity().y <= 0.0f) {
		anima->SetIsLoop(true);
		anima->SetAnimetion("Fall", 0.1f);
	}
	else {	// 上昇しているならアニメーションを変える
		anima->SetIsLoop(false);
		anima->SetAnimetion("JumpStrat1", 0.05f);
	}


	

	


}

// 終了
void PlayerStateJump::Exit() {
	character_->GetMoveComponent()->Velocity() = { 0,0,0 };
}
// 初期化
void PlayerStateJump::Enter() {


}

#pragma endregion // ジャンプ

#pragma region Attack

void PlayerStateAttack::Update()
{
	BaseWeapon* weapon = character_->GetWeapon();
	
	// コンボ
	weapon->GetComboStateMachine()->Update(character_->GetTime());
}

void PlayerStateAttack::Exit()
{
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	// 武器
	character_->GetWeapon()->GetComboStateMachine()->HandleInput(AttackInput::Light); // 弱攻撃
	character_->GetWeapon()->GetObject3D()->SetIsDraw(false);// 武器描画しない
	character_->GetWeapon()->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, false); // 武器コライダ無効

	// アニメーション
	anima->SetIsLoop(true);		   // ループ再生
	anima->SetIsPlaying(true);	   // 再生
	anima->SetAnimationSpeed(1.0f);// アニメーションスピード設定
}

void PlayerStateAttack::Enter()
{
	BaseWeapon* weapon = character_->GetWeapon();

	// 武器
	weapon->GetComboStateMachine()->Update(character_->GetTime());	// コンボステートマシーン更新
	weapon->GetObject3D()->SetIsDraw(true);	 // 武器描画
	weapon->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);	// 武器コライダ有効
	weapon->GetColliderComponent()->contactRecord_.Clear();	// 履歴削除
}

#pragma endregion // 攻撃

#pragma region Special



void PlayerStateSpecial::Update()
{
	BaseSpecial* special = character_->GetSpecial();
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	// キャラクターの動きを止める
	character_->Velocity() = {};
	int time = 0;
	// UIを表示しない
	player->GetPlayerUI()->SetIsTextRB(false);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special);
	rengeSp->InAction();	// アクション中
	rengeSp->SetIsDraw(false);	// 描画
	if (special->GetPhese() == 0) {	// 最初フェーズなら
		// 移動
		player->GetMoveComponent()->SetCanMove(true);
		// UI描画
		player->GetPlayerUI()->SetIsTextRB(true);
		// スペシャル描画
		rengeSp->SetIsDraw(true);
	}
	if (special->GetPhese() == 2) {	// 最終フェーズなら
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}
}

void PlayerStateSpecial::Exit()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetPlayerUI()->SetIsTextRB(false);
}

void PlayerStateSpecial::Enter()
{
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	player->GetSpecial()->SetPhese(0);	// フェーズ初期化
	player->GetSpecial()->SetGauge(0);	// ゲージ初期化
	player->GetWeapon()->GetObject3D()->SetIsDraw(false);	// 描画しない
}

#pragma endregion // 必殺技

#pragma region Skill
// 更新
void PlayerStateSkill::Update() {
	// 時間更新
	timer_ += character_->GetTime();

	// 移動に移行
	if (changeTimer_ <= timer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

};

// 終了
void PlayerStateSkill::Exit() {

};
// 初期化
void PlayerStateSkill::Enter() {
	BasePlayer* player = dynamic_cast<BasePlayer*>(character_);

	// スタン弾発射
	BulletInfo bulletInfo = {};
	bulletInfo.position = player->GetWorldTransform().worldMat_.GetWorldPosition();
	timer_ = 0.0f;
	player->GetBulletManager()->GenerateBullet(BulletManager::BulletType::kPlayerStan, bulletInfo);
};
#pragma endregion // スキル

#pragma region Defense

// 更新
void PlayerStateDefense::Update() {
	Input* input = character_->GetInput();

	// ゲームパッドがつながっているなら
	if (input->IsControllerConnected()) {
		// 防御中
		if (input->IsGamePadPressed(GamePadButton::GAMEPAD_A) && isDifense_) {
			isDifense_ = true;

			character_->GetCombatStatComponent()->damageReduction_ = 0.75;

			// スタミナがあるなら
			if (character_->GetCharacterParameterComponent().IsGetStamina()) {
				// スタミナ消費
				character_->GetCharacterParameterComponent().Stamina().rateFluctuation = -5.0f; 
			}
			else {
				isDifense_ = false;
			}

		}
		else {
			// スタミナ回復
			character_->GetCharacterParameterComponent().Stamina().rateFluctuation = 5.0f;
			isDifense_ = false;
			timer_ += character_->GetTime();
			character_->GetCombatStatComponent()->damageReduction_ = 0.0f;
		}
	}


	// 移動状態に移行
	if (timer_ >= defenseTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}

};

// 終了
void PlayerStateDefense::Exit() {
	isDifense_ = false;
	character_->GetCombatStatComponent()->damageReduction_ = 0.0f;
	character_->GetCharacterParameterComponent().Stamina().rateFluctuation = 5.0f;
};
// 初期化
void PlayerStateDefense::Enter() {
	isDifense_ = true;
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(false);		// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
	anima->SetAnimetion("Defense1", 0.10f);	// 流すアニメーション設定
};

#pragma endregion // 防御

#pragma region Fainting

// 更新
void PlayerStateFainting::Update() {
	Input* input = character_->GetInput();
	Vector2 left = input->GetGamePadLeftStick();

	timer_ += character_->GetTime();

	// スティックを動かしているなら
	if (prevleftStick != left) {
		Vector2 sub = prevleftStick - left;
		float length = sub.Length();
		float subtime = 1.0f;

		if (length >= 10.0f) {
			subtime = 2.0f;
		}
		
		// 復帰時間短縮
		timer_ += character_->GetTime() * subtime;
	}

	// 復帰したら移動状態に移行
	if (timer_ >= faintingTimer_) {
		character_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		return;
	}


	prevleftStick = left;
}

// 終了
void PlayerStateFainting::Exit() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(true);			// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
};
// 初期化
void PlayerStateFainting::Enter() {
	AnimationComponent* anima = character_->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	anima->SetIsPlaying(true);		// アニメーション再生
	anima->SetIsLoop(true);			// アニメーションをループさせるか
	anima->SetStratAnimeTime();		// アニメーション時間を初期化
	anima->SetAnimationSpeed(0.75f); // アニメーションスピード設定
	anima->SetAnimetion("Stan1", 0.10f);
	timer_ = 0;		// タイマーを0に設定


	Input* input = character_->GetInput();
	prevleftStick = input->GetGamePadLeftStick();
};

#pragma endregion // 気絶



