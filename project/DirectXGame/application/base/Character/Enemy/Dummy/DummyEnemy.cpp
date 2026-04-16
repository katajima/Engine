#include "DummyEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void DummyEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySG01.obj", "enemy");
		// サイズ
		Vector3 size = { 0.9f,0.9f,0.9f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		moveComponent_->GetMoveSystem()->Data().maxSpeed = 0.0f;


		objectComponentShadow_->GetWorldTransform().scale_ = { 5.0f,5.0f ,5.0f };

		
		// パラメーター初期化
		Parameters()->HP.Initiaize(hp, 0, hp, 0);
		Parameters()->strength = 10.0f;

		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };

		// 保存項目初期化
		InitializeBaseAddItem();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void DummyEnemy::Update() {
		// 基盤の更新
		BaseUpdate();

		Parameters()->HP.value = 100.0f;
	}


	void DummyEnemy::Emit() {
		// エフェクト座標更新
		worldEffect_.Update();

		// 各エフェクト出現
		effect->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
		effect->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
	}

	void DummyEnemy::InitStateMachine() {
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<DummyEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<DummyEnemyDieState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}
}