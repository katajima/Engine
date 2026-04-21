#include "MediumMeleeEnemy.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
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
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<MediumMeleeEnemyDamageState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}

	void MediumMeleeEnemy::Update() {
		// 基盤の更新
		BaseUpdate();
	}

	void MediumMeleeEnemy::Draw2D(){}

	void MediumMeleeEnemy::Move(){
		if (isStopping_) return;

		// 位置差分
		Vector3 toTarget = Subtract(GetTargetPos(), GetWorldTransform().translate_);
		toTarget.y = 0.0f;

		float distance = toTarget.Length();
		if (distance <= 0.001f) {
			moveComponent_->GetMoveSystem()->Data().maxSpeed = 0.0f;
			return;
		}

		// 水平向き
		Vector3 dire = toTarget.Normalize();

		// 回転設定（即値代入ではなく補間推奨）
		Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

		// Y軸だけゆっくり向く
		float currentY = GetWorldTransform().rotate_.y;
		float targetY = rotate.y;

		// 必要なら AngleDiff のような角度差補正関数を使う
		float diff = targetY - currentY;

		// -π～πへ補正
		while (diff > std::numbers::pi_v<float>) {
			diff -= std::numbers::pi_v<float> *2.0f;
		}
		while (diff < -std::numbers::pi_v<float>) {
			diff += std::numbers::pi_v<float> *2.0f;
		}

		// 回転速度
		float turnSpeed = globalData_.turnSpeed * GetTime();
		diff = std::clamp(diff, -turnSpeed, turnSpeed);

		GetWorldTransform().rotate_.y = currentY + diff;

		// 基本速度
		float targetSpeed = 0.0f;

		// 距離ごとの行動分岐
		if (distance <= globalData_.startRetreatingRadius) {
			// 近すぎるので少し下がる
			attackTimer_ = 0.0f;
			targetSpeed = -globalData_.retreatSpeed;
		}
		else if (distance <= globalData_.attackStartRadius) {
			// 攻撃準備距離
			attackTimer_ += GetTime();

			// 完全停止だと不自然なら微速前進か停止
			targetSpeed = 0.0f;

			if (attackTimer_ >= globalData_.attackTimer) {
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
				attackTimer_ = 0.0f;
				return;
			}
		}
		else {
			// 接近
			attackTimer_ = 0.0f;
			targetSpeed = moveSpeed_;
		}

		moveComponent_->GetMoveSystem()->Data().maxSpeed = targetSpeed;
	}

	void MediumMeleeEnemy::InitParticle(){
		Engine::ParticleManager* particleManager = entityManager->GetEffectManager()->GetParticleManager();

		// エフェクト用のトランスフォーム初期化
		worldEffect_.Initialize();
		worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_.translate_ = { 0,1,0 };
	}
}
