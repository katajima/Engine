#include "NormalEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

void NormalEnemy::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	// 基盤初期化
	BaseInitialize(input,entity3DManager,entity2DManager,globalVariables,position,camera, "enemy.gltf", "enemy");
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
	// パーティクル初期化
	InitParticle();
	// トランスフォーム更新
	GetWorldTransform().Update();
}

void NormalEnemy::InitStateMachine() {
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateMove>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateAttack>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
		return std::make_unique<EnemyStateDie>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
}

void NormalEnemy::Update(){
	// 基盤の更新
	BaseUpdate();
}

void NormalEnemy::Draw2D()
{
	if (GetObjectComponent() == nullptr) { return; }

	// ロックオンされているなら
	if (GetIsLockOn()) {
		Vector2 screenPos = objectComponent_->GetScreenPosition();	// スクリーン座標取得
		icon_lockOn->SetPosition(screenPos + Vector2{ 0.0f,-40.0f });// 位置設定

		icon_lockOn->Update();	// 更新
		icon_lockOn->Draw();	// 描画
	}
}

void NormalEnemy::Emit()
{
	// エフェクト座標更新
	worldEffect_.Update();

	// 各エフェクト出現
	effect_->Emit("starEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEmit", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("hitEffect2", worldEffect_.worldMat_.GetWorldPosition());
	effect_->Emit("ringHit", worldEffect_.worldMat_.GetWorldPosition());
}

void NormalEnemy::Move()
{
	// 移動
	DirectionMove(Parameters().speed);
}

void NormalEnemy::InitParticle()
{
	Engine::ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	// エフェクト用のトランスフォーム初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };
}

