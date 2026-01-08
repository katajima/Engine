#include "BulletEnemy.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include"DirectXGame/application/base/Effect/Effect.h"

// 初期化
void BulletEnemy::Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {

	// 基盤初期化
	BaseInitialize(input, entity3DManager, entity2DManager, globalVariables, position, camera, "enemyBullet.gltf", "enemyBullet");
	// サイズ
	Vector3 size = { 1.0f,1.0f,1.0f };
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
};

void BulletEnemy::InitStateMachine() {
	// ステートマシーン初期化
	stateMachine_ = std::make_unique<CharacterStateMachine>();
	stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
		return std::make_unique<BulletEnemyStateMove>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Attack, [](BaseCharacter* p) {
		return std::make_unique<BulletEnemyStateAttack>(p);
		});
	stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
		return std::make_unique<BulletEnemyStateDie>(p);
		});
	stateMachine_->Init(this, CharacterMainState::Move);
}

// 毎フレーム更新
void BulletEnemy::Update() {

};
// 描画2d
void BulletEnemy::Draw2D() {
};
// エミッター出現
void BulletEnemy::Emit() {
};
// 移動
void BulletEnemy::Move() {
	// 移動
	DirectionMove(Parameters().speed);
};


// パーティクル初期化
void BulletEnemy::InitParticle() {
	Engine::ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	// エフェクト用のトランスフォーム初期化
	worldEffect_.Initialize();
	worldEffect_.parent_ = &objectComponent_->GetWorldTransform();
	worldEffect_.translate_ = { 0,1,0 };
};