#include "LargeMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void LargeMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entity3DManager,  globalVariables, position, camera, "enemy.gltf", "enemy", 4);
		// サイズ
		Vector3 size = { 3.75f,3.75f,3.75f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		// パラメーター初期化
		Parameters()->HP.Initiaize(100, 0, 100, 0);
		Parameters()->strength = 10.0f;

		
		// 保存項目初期化
		InitializeBaseAddItem();
		// パーティクル初期化
		InitParticle();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void LargeMeleeEnemy::Update() {}

	void LargeMeleeEnemy::Draw2D() {}

	void LargeMeleeEnemy::Move() {}

	void LargeMeleeEnemy::InitStateMachine() {}

	void LargeMeleeEnemy::InitParticle() {}
}