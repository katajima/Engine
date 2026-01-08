#include "AttackController.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>

void AttackController::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, 
	CharacterParameterComponent* base, BaseCharacter* owner) {
	this->globalVariables = globalVariables;

	// ダメージ計算用ステータスの初期化
	combatStat_ = std::make_unique<CombatStat>();
	combatStat_->Initialize(base);

	// コンボシステムの初期化
	comboSystem_ = std::make_unique<ComboSystem>();
	comboSystem_->Initialize(owner,globalVariables);

	// ヒットボックスシステム
	hitBoxSystem_ = std::make_unique<HitBoxSystem>();
	hitBoxSystem_->Initialize(owner,entity3DManager);
};


void AttackController::Update(float dt) {


	// ヒットカウンターの更新
	if (!isStopHitTimer_) {
		hitCounter_.Update(dt);
	}
	// ヒットボックスシステムの更新
	hitBoxSystem_->Update(dt);
	if (IsAttack()) {
		// コンボシステムの更新
		comboSystem_->UpdateCombo(dt);
	}
};
