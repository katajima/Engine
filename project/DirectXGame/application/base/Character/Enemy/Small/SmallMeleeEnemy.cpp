#include "SmallMeleeEnemy.h"

void SmallMeleeEnemy::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	// 基盤初期化
	BaseInitialize(input, entity3DManager, entity2DManager, globalVariables, position, camera, "enemyBodySG01.obj", "enemy");
	// サイズ
	Vector3 size = { 1.7f,1.7f,1.7f };
	objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定


	// パラメーター初期化
	Parameters().HP.Initiaize(100, 0, 100, 0);
	Parameters().speed = 10.0f;
	Parameters().strength = 10.0f;

	moveComponent_->GetMoveSystem()->GetData().maxSpeed = Parameters().speed;

	// 保存項目初期化
	InitializeBaseAddItem();
	// スプライト初期化
	Initialize2D();
	// トランスフォーム更新
	GetWorldTransform().Update();
}

void SmallMeleeEnemy::Update()
{
}

void SmallMeleeEnemy::Draw2D()
{
}

void SmallMeleeEnemy::Emit()
{
}

void SmallMeleeEnemy::Move()
{
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
