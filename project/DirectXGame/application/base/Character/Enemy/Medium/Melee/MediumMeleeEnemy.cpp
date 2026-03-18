#include "MediumMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void MediumMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera,
			"enemy.gltf", "MediumMeleeEnemy", 1.5f);

		objectComponentShadow_->GetWorldTransform().scale_ = { 2.0f,2.0f ,2.0f };


		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		// パラメーター初期化
		parameterComponent_->parameters->HP.Initiaize(50, 0, 50, 0);
		parameterComponent_->parameters->strength = 10.0f;

		moveSpeed_ = moveComponent_->GetMoveSystem()->Data().maxSpeed;
		// パーティクル初期化
		InitParticle();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void MediumMeleeEnemy::InitStateMachine() {
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyAttackState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyDieState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}

	void MediumMeleeEnemy::Update() {
		// 基盤の更新
		BaseUpdate();
	}

	void MediumMeleeEnemy::Draw2D(){}

	void MediumMeleeEnemy::Emit(){
		// エフェクト座標更新
		worldEffect_.Update();

		// 各エフェクト出現
		effect->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
	}

	void MediumMeleeEnemy::Move(){
		// 距離設定
		Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
		dire.y = 0.0f;
		// 回転設定
		Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);
		// Y軸周り角度
		GetWorldTransform().rotate_.y = rotate.y;

		if (GetTargetDistance() <= globalData_.attackStartRadius) {
			attackTimer_ += GetTime();
			moveComponent_->GetMoveSystem()->Data().maxSpeed = 0;
			if (attackTimer_ >= globalData_.attackTimer) {
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
				attackTimer_ = 0.0f;
				return;
			}
			if (GetTargetDistance() <= globalData_.startRetreatingRadius) {
				moveComponent_->GetMoveSystem()->Data().maxSpeed = -globalData_.retreatSpeed;
			}
		}
		else {
			attackTimer_ = 0.0f;
			moveComponent_->GetMoveSystem()->Data().maxSpeed = moveSpeed_;
		}
	}

	void MediumMeleeEnemy::InitParticle(){
		Engine::ParticleManager* particleManager = entityManager->GetEffectManager()->GetParticleManager();

		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };
	}
}
