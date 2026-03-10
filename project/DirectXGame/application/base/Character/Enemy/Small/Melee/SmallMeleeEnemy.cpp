#include "SmallMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void SmallMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySG01.obj", "smallMeleeEnemy");
		
		
		
		// サイズ
		Vector3 size = { 1.0f,1.0f,1.0f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		objectComponentShadow_->GetWorldTransform().scale_ = { 2.0f,2.0f ,2.0f };

		
		moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters()->speed;
		moveSpeed_ = moveComponent_->GetMoveSystem()->GetData().maxSpeed;


		// 武器
		weapon_ = std::make_unique<SmallMeleeWeapon>();
		weapon_->SetCharacter(this);
		weapon_->Initialize(inputSystem, entityManager, globalVariables, {}, nullptr);
		weapon_->GetWorldTransform().parent_ = &objectComponent_->GetWorldTransform();
		weapon_->GetWorldTransform().translate_ = { 0.0f,0.0f,1.0f };


		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };

		// スプライト初期化
		Initialize2D();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void SmallMeleeEnemy::Update() {
		// 攻撃制御更新
		attackController_->Update(GetTime());
		// 基盤の更新
		BaseUpdate();
		//
		weapon_->Update(); // 武器更新
	}

	void SmallMeleeEnemy::Draw2D() {
	}

	void SmallMeleeEnemy::Emit() {
		// エフェクト座標更新
		worldEffect_.Update();

		// 各エフェクト出現
		effect->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
	}

	void SmallMeleeEnemy::Move() {
		// 距離設定
		Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
		// 回転設定
		Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);
		// Y軸周り角度
		GetWorldTransform().rotate_.y = rotate.y;
		if (GetTargetDistance() <= globalData_.attackStartRadius) {
			attackTimer_ += GetTime();
			Parameters()->speed = 0;
			if (attackTimer_ >= globalData_.attackTimer) {
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
				attackTimer_ = 0.0f;
				return;
			}
			if (GetTargetDistance() <= globalData_.startRetreatingRadius) {
				Parameters()->speed = -globalData_.retreatSpeed;
			}
		}
		else {
			attackTimer_ = 0.0f;
			Parameters()->speed = moveSpeed_;
		}
	}

	void SmallMeleeEnemy::InitStateMachine()
	{
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<SmallMeleeEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
			return std::make_unique<SmallMeleeEnemyAttackState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<SmallMeleeEnemyDieState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}
}