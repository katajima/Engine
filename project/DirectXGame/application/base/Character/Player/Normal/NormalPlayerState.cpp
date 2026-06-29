#include "NormalPlayerState.h"
#include "NormalPlayer.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/application/base/Bullet/Base/BulletManager.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include <DirectXGame/engine/Animation/AnimationComponent.h>
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>
#include <DirectXGame/application/base/Camera/Base/CameraManeger.h>

namespace Character {
#pragma region Idle

	// 更新
	void PlayerStateIdle::Update(const CharacterContext& ctx) {
		BaseWeapon* weapon = character->GetWeapon();
		BaseSpecial* special = character->GetSpecial();
		character->GetWeapon()->SetTrailEmit(false);


		// 武器描画 
		weapon->GetObject3D()->SetIsDraw(true);

		// 必殺技移行
		if (special->GetIsSpecial()) {
			if (ctx.inputData.specialTrigger) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
				return;
			}
		}

		// 回避入力があれば移動方向または向いている方向へステップする
		if (ctx.inputData.dodgeTrigger && character->GetCharacterParameterComponent()->GetStamina() >= 20.0f) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Avoidance);
			return;
		}

		// 移動したら
		if (ctx.inputData.moveShick.Length() != 0) {
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

		float blendTime = 0.1f;
		anima->SetAnimation("SwordIdle01", blendTime);
	};

#pragma endregion // 待機

#pragma region Move

	void PlayerStateMove::Update(const CharacterContext& ctx)
	{
		BaseWeapon* weapon = character->GetWeapon();		// 武器
		BaseSpecial* special = character->GetSpecial();	// 必殺
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		weapon->GetObject3D()->SetIsDraw(true); // 武器描画
		anima->SetAnimation("SwordRun01", 0.1f);	// 流すアニメーション設定

		// 必殺がうてるなら
		if (special->GetIsSpecial()) {
			if (ctx.inputData.specialTrigger) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Special);
				return;
			}
		}

		// 回避入力があれば移動方向または向いている方向へステップする
		if (ctx.inputData.dodgeTrigger && character->GetCharacterParameterComponent()->GetStamina() >= 20.0f) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Avoidance);
			return;
		}

		// 止まったら
		if (ctx.inputData.moveShick.Length() == 0) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
			return;
		}
		// アニメーションスピード設定
		float speed = character->GetMoveComponent()->GetMoveSystem()->GetAnimationSpeed();

		anima->SetAnimationSpeed(speed);
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
	}

#pragma endregion // 移動

#pragma region Jump

	// 更新
	void PlayerStateJump::Update(const CharacterContext& ctx) {
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		JumpSystem* jump = character->GetMoveComponent()->GetJumpSystem();
		timer += ctx.dt;

		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定


		// ジャンプ出来るか
		bool isJamp = character->GetMoveComponent()->GetIsJump();
		bool isTrigger = ctx.inputData.jumpTrigger;
		bool isPress = ctx.inputData.jumpPressed;


		bool isTriggerLT = ctx.inputData.dashHeld;
		


		bool isAlive = character->GetAlive();



		// ジャンプ入力をセット
		jump->SetInputPressed(isPress);

		// キャラクターが生きていてジャンプ回数が残っていて着地状態じゃないのなら
		if (isAlive && isJamp && isTrigger) {
			character->GetObjectComponent()->GetObject3D()->GetAnimationComponent()->SetAnimation("JumpStart01", 0.1f);
			// ジャンプ開始
			anima->SetStratAnimeTime();		// アニメーション時間を初期化
			jump->StartJump(*character->GetObjectComponent()->GetRigidBodyComponent());
		}
		else {
			// 着地状態なら
			if (maxTimer <= timer) {
				if (character->GetMoveComponent()->GetIsLanding()) {
					character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
					return;
				}
			}
		}



		if (character->GetMoveComponent()->GetVelocity().y > 0.0f) { // 上昇しているなら
			anima->SetIsLoop(false);
			anima->SetAnimation("JumpStart01", 0.05f);
		}
		else { // 降下しているなら
			anima->SetIsLoop(true);
			anima->SetAnimation("Jump01", 0.15f);
		}
	}

	// 終了
	void PlayerStateJump::Exit() {
		character->GetMoveComponent()->GetVelocity() = { 0,0,0 };
		timer = 0.0f;
	}
	// 初期化
	void PlayerStateJump::Enter() {
		timer = 0.0f;
	}

#pragma endregion // ジャンプ

#pragma region Avoidance

	void PlayerStateAvoidance::Update(const CharacterContext& ctx) {
		timer_ += ctx.dt;

		if (!isDirectionFixed_) {
			// 入力方向がある時はその方向、無い時は現在の向きを回避方向に使う
			direction_ = Vector3{ ctx.worldStickDirection.x,0.0f,ctx.worldStickDirection.y };
			if (direction_.Length() == 0.0f) {
				direction_ = ctx.direction;
				direction_.y = 0.0f;
			}
			direction_ = Normalize(direction_);
			isDirectionFixed_ = true;
		}

		MoveRequest request{};						// 回避専用の移動リクエスト
		request.layer = MoveLayer::kOverride;		// 通常移動より優先したい移動
		request.velocity = direction_ * speed_ * ctx.dt;	// 1フレーム分の回避移動量
		request.direction = direction_;				// 回避方向へ体を向ける
		request.priority = 0;						// 通常移動より優先度を高くする
		request.invincible = true;					// 将来の無敵判定拡張用フラグ
		character->GetMoveComponent()->GetMoveRequestSystem()->SetRequest(request);

		// 回避時間が終わったら入力状態に合わせて自然な通常ステートへ戻す
		if (timer_ >= duration_) {
			if (ctx.inputData.moveShick.Length() != 0.0f) {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			}
			else {
				character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
			}
			return;
		}
	}

	void PlayerStateAvoidance::Exit() {
		timer_ = 0.0f;				// 次回回避用に経過時間を戻す
		isDirectionFixed_ = false;	// 次回回避用に方向決定を解除する
	}

	void PlayerStateAvoidance::Enter() {
		timer_ = 0.0f;				// 回避開始時にタイマーを初期化する
		isDirectionFixed_ = false;	// 初回更新で入力方向を確定する
		character->GetCharacterParameterComponent()->Stamina().Add(-staminaCost_);	// 回避コストを支払う
		character->GetCharacterParameterComponent()->Stamina().useRate = false;		// 回避中はスタミナ回復を止める

		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsLoop(false);					// 回避は短い単発モーションとして扱う
		anima->SetIsPlaying(true);					// アニメーション再生
		anima->SetAnimationSpeed(1.5f);				// 既存走りモーションを素早く再生する
		anima->SetAnimation("Rig|Roll", 0.05f);	// 専用回避アニメが無いので走りを短くブレンドする
	}

#pragma endregion // 回避

#pragma region Attack

	void PlayerStateAttack::Update(const CharacterContext& ctx) {}

	void PlayerStateAttack::Exit()
	{
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		// 武器描画しない
		character->GetWeapon()->GetObject3D()->SetIsDraw(false);

		character->GetCameraManager()->GetBaseCamera()->LockOn(nullptr);

		// アニメーション
		anima->SetIsLoop(true);		   // ループ再生
		anima->SetIsPlaying(true);	   // 再生
		anima->SetAnimationSpeed(1.0f);// アニメーションスピード設定
	}

	void PlayerStateAttack::Enter()
	{
		BaseWeapon* weapon = character->GetWeapon();
		// 武器
		weapon->GetObject3D()->SetIsDraw(true);	 // 武器描画
	}

#pragma endregion // 攻撃

#pragma region Special



	void PlayerStateSpecial::Update(const CharacterContext& ctx)
	{
		BaseSpecial* special = character->GetSpecial();
		BasePlayer* player = dynamic_cast<BasePlayer*>(character);

		// UIを表示しない
		player->GetPlayerUI()->SetIsTextRB(false);
		RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special);

		rengeSp->InAction();	// アクション中
		rengeSp->SetIsDraw(false);	// 描画
		if (special->GetPhese() == 0) {	// 最初フェーズなら
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

#pragma region Fainting

	// 更新
	void PlayerStateFainting::Update(const CharacterContext& ctx) {
		///Rig|Death01
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
	};

#pragma endregion 

#pragma region Die

	void PlayerStateDie::Update(const CharacterContext& ctx) {
	
	}

	void PlayerStateDie::Exit() {
	}
	
	void PlayerStateDie::Enter() {
		character->GetDeathSystem()->StartDeath(DeathType::Normal, { 1.0f ,false,1.0f,{} });
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetIsLoop(false);			// アニメーションをループさせるか
		anima->SetStratAnimeTime();		// アニメーション時間を初期化
		anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
		anima->SetAnimation("Rig|Death01", 0.1f);	// 流すアニメーション設定

	}

#pragma endregion // 死亡

	void PlayerStateDamage::Update(const CharacterContext& ctx) {
		timer += ctx.dt;
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		if (timer >= anima->GetEndAnimeTime("Rig|Hit_Chest")) {
			character->GetCharacterStateMachine()->ChangeState(CharacterMainState::Idle);
		}
	}

	void PlayerStateDamage::Exit() {
	}

	void PlayerStateDamage::Enter() {
		timer = 0.0f;
		Engine::AnimationComponent* anima = character->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		anima->SetIsPlaying(true);		// アニメーション再生
		anima->SetIsLoop(false);			// アニメーションをループさせるか
		anima->SetStratAnimeTime();		// アニメーション時間を初期化
		anima->SetAnimationSpeed(1.0f); // アニメーションスピード設定
		anima->SetAnimation("Rig|Hit_Chest", 0.1f);	// 流すアニメーション設定

		character->GetEffect()->Emit("EmitterPlayerHitDamageEffect", character->GetWorldPosition() + hitEffectPos);
		character->GetEffect()->Emit("EmitterRingHit", character->GetWorldPosition() + hitEffectPos);

	}

}


