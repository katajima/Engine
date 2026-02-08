#include "AttackController.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>

void AttackController::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,
	CharacterParameterComponent* base, BaseCharacter* owner) {
	this->globalVariables = globalVariables;

	// ダメージ計算用ステータスの初期化
	combatStat_ = std::make_unique<CombatStat>();
	combatStat_->Initialize(base);

	// コンボシステムの初期化
	comboSystem_ = std::make_unique<Combo::System>();
	comboSystem_->Initialize(owner, globalVariables);

	// ヒットボックスシステム初期化
	hitBoxSystem_ = std::make_unique<HitBox::System>();
	hitBoxSystem_->Initialize(owner, entity3DManager);

	// ロックオンシステム初期化
	lockOnSystem_ = std::make_unique<LockOnSystem>();
	lockOnSystem_->Initialize(owner);

};


void AttackController::Update(float dt) {


	// ヒットカウンターの更新
	if (!isStopHitTimer_) {
		hitCounter_.Update(dt);
	}

	// ロックオンシステム更新
	//lockOnSystem_->Update(dt);

	// ヒットボックスシステムの更新
	hitBoxSystem_->Update(dt);
	if (IsAttack() || isDebugEditor_) {
		// コンボシステムの更新
		comboSystem_->UpdateCombo(dt);
	}
};
