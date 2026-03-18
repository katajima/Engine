#include "SmallCartEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void SmallCartEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySG01.obj", "enemy");
		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f, };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		// パラメーター初期化
		Parameters()->HP.Initiaize(75, 0, 100, 0);
		Parameters()->strength = 10.0f;

		moveSpeed_ = moveComponent_->GetMoveSystem()->Data().maxSpeed;
		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };

		// 保存項目初期化
		InitializeBaseAddItem();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void SmallCartEnemy::Update() {
		// 基盤の更新
		BaseUpdate();
	}

	void SmallCartEnemy::Draw2D() {}

	void SmallCartEnemy::Emit() {
		// エフェクト座標更新
		worldEffect_.Update();

		// 各エフェクト出現
		effect->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
	}

	void SmallCartEnemy::Move() {
		// 距離設定
		Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
		// 回転設定
		Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);
		// Y軸周り角度
		GetWorldTransform().rotate_.y = rotate.y;
		if (GetTargetDistance() <= 25.0f) {
			attackTimer_ += GetTime();
			if (attackTimer_ >= 3.0f) {
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
				attackTimer_ = 0.0f;
				return;
			}
			if (GetTargetDistance() <= 20.0f) {
				moveComponent_->GetMoveSystem()->Data().maxSpeed = -moveSpeed_ * 1.5f;
			}
		}
		else {
			attackTimer_ = 0.0f;
			moveComponent_->GetMoveSystem()->Data().maxSpeed = moveSpeed_;
		}
	}

	void SmallCartEnemy::InitStateMachine() {
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<SmallCartEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<SmallCartEnemyDieState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}
}