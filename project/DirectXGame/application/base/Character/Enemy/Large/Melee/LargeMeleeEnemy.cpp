#include "LargeMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

namespace Character {
	void LargeMeleeEnemy::Initialize(InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entity3DManager, entity2DManager, globalVariables, position, camera, "enemy.gltf", "enemy", 4);
		// サイズ
		Vector3 size = { 3.75f,3.75f,3.75f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


		// パラメーター初期化
		Parameters()->HP.Initiaize(100, 0, 100, 0);
		Parameters()->speed = 10.0f;
		Parameters()->strength = 10.0f;

		moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters()->speed;

		// 保存項目初期化
		InitializeBaseAddItem();
		// スプライト初期化
		Initialize2D();
		// パーティクル初期化
		InitParticle();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void LargeMeleeEnemy::Update()
	{
	}

	void LargeMeleeEnemy::Draw2D()
	{
	}

	void LargeMeleeEnemy::Emit()
	{
	}

	void LargeMeleeEnemy::Move()
	{
	}

	void LargeMeleeEnemy::InitStateMachine()
	{
	}

	void LargeMeleeEnemy::InitParticle() {
	};
}