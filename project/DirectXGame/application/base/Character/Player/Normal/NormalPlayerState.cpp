#include "NormalPlayerState.h"
#include "NormalPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>

namespace Character {
#pragma region Idle

	// 更新
	void PlayerStateIdle::Update() {
		Engine::Input* input = character->GetInput();
		BaseWeapon* weapon = character->GetWeapon();
		BaseSpecial* special = character->GetSpecial();
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		MoveSystem* move = character->GetMoveComponent()->GetMoveSystem();
		character->GetWeapon()->GetObject3D()->isEmitTrailEffect = false;




		bool isTriggerLT = input->IsLeftTriggerPressed();
		if (isTriggerLT) {
			character->GetMoveComponent()->GetDashSystem()->StartDash();
		}


		// 武器描画 
		weapon->GetObject3D()->SetIsDraw(true);

		// ゲームパッドが繋いであるなら
		if (input->IsControllerConnected()) {
			// スキル発動
			if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_X)) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Skill);
				return;
			}
		}

#ifdef _DEBUG

		// デバッグ用
		if (input->IsTriggerKey(DIK_Z)) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
			return;
		}
#endif // _DEBUG

		weapon->RecastTime(Engine::MyGame::GameTime());

		// 必殺技移行
		if (special->GetIsSpecial()) {
			if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB)) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
				return;
			}
		}

		// 移動したら
		if (input->GetGamePadLeftStick().Length() != 0) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}
	};

	// 終了
	void PlayerStateIdle::Exit() {
	};

	// 初期化
	void PlayerStateIdle::Enter() {
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsLoop(true);			// ループさせる
		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetAnimationSpeed(1.0f);	//　アニメーションスピード設定
		anima->SetAnimation("SwordIdle01", 0.5f);	// 流すアニメーション設定
		character->GetMoveComponent()->SetCanMove(true);
	};

#pragma endregion // 待機

#pragma region Move

	void PlayerStateMove::Update()
	{
		Engine::Input* input = character->GetInput();				// 入力
		BaseWeapon* weapon = character->GetWeapon();		// 武器
		BaseSpecial* special = character->GetSpecial();	// 必殺
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		character->GetMoveComponent()->SetCanMove(true);
		weapon->GetObject3D()->SetIsDraw(true); // 武器描画
		anima->SetAnimation("SwordRun01", 0.1f);	// 流すアニメーション設定

		bool isTriggerLT = input->IsLeftTriggerPressed();
		if (isTriggerLT) {
			character->GetMoveComponent()->GetDashSystem()->StartDash();
		}




		// 気絶発動
		if (input->IsTriggerKey(DIK_Z)) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
			return;
		}

		// 武器リキャストタイム更新
		weapon->RecastTime(Engine::MyGame::GameTime());

		// 必殺がうてるなら
		if (special->GetIsSpecial()) {
			if (input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB)) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
				return;
			}
		}

		// 止まったら
		if (input->GetGamePadLeftStick().Length() == 0) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
			return;
		}
		// アニメーションスピード設定
		float speed = character->GetMoveComponent()->GetMoveSystem()->GetAnimationSpeed();

		anima->SetAnimationSpeed(speed);
		//}
	}

	void PlayerStateMove::Exit()
	{
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetAnimationSpeed(1.0f);	//　アニメーションスピード設定
	}

	void PlayerStateMove::Enter()
	{
		BaseWeapon* weapon = character->GetWeapon();
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		weapon->GetObject3D()->SetIsDraw(false);	// 武器描画しない
		anima->SetIsLoop(true);				// ループ再生
		anima->SetIsPlaying(true);			// 再生
		anima->SetAnimationSpeed(1.0f);		// アニメーションスピード設定
		anima->SetAnimation("SwordRun01", 0.1f);	// 流すアニメーション設定
		character->GetMoveComponent()->SetCanMove(true);
	}

#pragma endregion // 移動

#pragma region Jump

	// 更新
	void PlayerStateJump::Update() {
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		Engine::Input* input = character->GetInput();
		JumpSystem* jump = character->GetMoveComponent()->GetJumpSystem();

		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定


		// ジャンプ出来るか
		bool isJamp = character->GetMoveComponent()->GetIsJump();
		bool isTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_A);
		bool isPress = input->IsGamePadPressed(GamePadButton::GAMEPAD_A);

		bool isTriggerLT = input->IsLeftTriggerPressed();
		if (isTriggerLT) {
			character->GetMoveComponent()->GetDashSystem()->StartDash();
		}


		bool isAlive = character->GetAlive();



		// ジャンプ入力をセット
		jump->SetInputPressed(isPress);

		// キャラクターが生きていてジャンプ回数が残っていて着地状態じゃないのなら
		if (isAlive && isJamp && isTrigger) {
			character->GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimation("JumpStart01", 0.05f);

			// ジャンプ開始
			anima->SetStratAnimeTime();		// アニメーション時間を初期化
			jump->StartJump(*character->GetObjectComponent()->GetRigidBodyComponent());
		}
		else {
			// 着地状態なら
			if (character->GetMoveComponent()->GetIsLanding()) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
				return;
			}
		}



		if (jump->GetState() == JumpSystem::State::Jump) { // 上昇しているなら
			anima->SetIsLoop(false);
			anima->SetAnimation("JumpStart01", 0.05f);
		}
		else if (jump->GetState() == JumpSystem::State::Fall) { // 降下しているなら
			anima->SetIsLoop(true);
			anima->SetAnimation("Jump01", 0.15f);
		}
	}

	// 終了
	void PlayerStateJump::Exit() {
		character->GetMoveComponent()->Velocity() = { 0,0,0 };
	}
	// 初期化
	void PlayerStateJump::Enter() {


	}

#pragma endregion // ジャンプ

#pragma region Attack

	void PlayerStateAttack::Update() {}

	void PlayerStateAttack::Exit()
	{
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		// 武器
		character->GetAttackController()->GetComboSystem()->GetComboStateMachine()->HandleInput(AttackInput::Light); // 弱攻撃
		character->GetWeapon()->GetObject3D()->SetIsDraw(false);// 武器描画しない

		// アニメーション
		anima->SetIsLoop(true);		   // ループ再生
		anima->SetIsPlaying(true);	   // 再生
		anima->SetAnimationSpeed(1.0f);// アニメーションスピード設定
		character->GetMoveComponent()->SetCanMove(true);
		character->GetMoveComponent()->GetMoveSystem()->SetIsAttack(false);
		character->GetMoveComponent()->GetJumpSystem()->SetIsAttack(false);
	}

	void PlayerStateAttack::Enter()
	{
		BaseWeapon* weapon = character->GetWeapon();

		// 武器
		weapon->GetObject3D()->SetIsDraw(true);	 // 武器描画
		character->GetMoveComponent()->GetMoveSystem()->SetIsAttack(true);
		character->GetMoveComponent()->GetJumpSystem()->SetIsAttack(true);
	}

#pragma endregion // 攻撃

#pragma region Special



	void PlayerStateSpecial::Update()
	{
		BaseSpecial* special = character->GetSpecial();
		BasePlayer* player = dynamic_cast<BasePlayer*>(character);

		// キャラクターの動きを止める
		character->Velocity() = {};
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
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}
	}

	void PlayerStateSpecial::Exit()
	{
		BasePlayer* player = dynamic_cast<BasePlayer*>(character);

		player->GetPlayerUI()->SetIsTextRB(false);
	}

	void PlayerStateSpecial::Enter()
	{
		BasePlayer* player = dynamic_cast<BasePlayer*>(character);

		player->GetSpecial()->SetPhese(0);	// フェーズ初期化
		player->GetSpecial()->SetGauge(0);	// ゲージ初期化
		player->GetWeapon()->GetObject3D()->SetIsDraw(false);	// 描画しない
	}

#pragma endregion // 必殺技

#pragma region Skill
	// 更新
	void PlayerStateSkill::Update() {
		// 時間更新
		timer_ += character->GetTime();

		// 移動に移行
		if (changeTimer_ <= timer_) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}

	};
	// 終了
	void PlayerStateSkill::Exit() {

	};
	// 初期化
	void PlayerStateSkill::Enter() {
		BasePlayer* player = dynamic_cast<BasePlayer*>(character);

		// スタン弾発射
		BulletInfo bulletInfo = {};
		bulletInfo.position = player->GetWorldTransform().worldMat_.GetWorldPosition();
		timer_ = 0.0f;
	};
#pragma endregion // スキル

#pragma region Fainting

	// 更新
	void PlayerStateFainting::Update() {
		Engine::Input* input = character->GetInput();
		Vector2 left = input->GetGamePadLeftStick();

		timer_ += character->GetTime();

		// スティックを動かしているなら
		if (prevleftStick != left) {
			Vector2 sub = prevleftStick - left;
			float length = sub.Length();
			float subtime = 1.0f;

			if (length >= 10.0f) {
				subtime = 2.0f;
			}

			// 復帰時間短縮
			timer_ += character->GetTime() * subtime;
		}

		// 復帰したら移動状態に移行
		if (timer_ >= faintingTimer_) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			return;
		}


		prevleftStick = left;
	}

	// 終了
	void PlayerStateFainting::Exit() {
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetIsLoop(true);			// アニメーションをループさせるか
		anima->SetStratAnimeTime();		// アニメーション時間を初期化
		anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
	};
	// 初期化
	void PlayerStateFainting::Enter() {
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetIsLoop(true);			// アニメーションをループさせるか
		anima->SetStratAnimeTime();		// アニメーション時間を初期化
		anima->SetAnimationSpeed(0.75f); // アニメーションスピード設定
		anima->SetAnimation("Stan1", 0.10f);
		timer_ = 0;		// タイマーを0に設定


		Engine::Input* input = character->GetInput();
		prevleftStick = input->GetGamePadLeftStick();
	};

#pragma endregion // 気絶
}


