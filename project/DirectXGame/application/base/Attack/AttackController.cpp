#include "AttackController.h"

void AttackController::Initialize(Entity3DManager* entity3DManager, CharacterParameterComponent* base, BaseCharacter* owner) {

	// ダメージ計算用ステータスの初期化
	combatStat_ = std::make_unique<CombatStat>();
	combatStat_->Initialize(base);

	// ヒットカウンターの初期化
	hitCounter_;

	// コンボシステムの初期化
	comboSystem_ = std::make_unique<ComboSystem>();
	comboSystem_->Initialize(owner);

	// ヒットボックスシステム
	hitBoxSystem_ = std::make_unique<HitBoxSystem>();
	hitBoxSystem_->Initialize(owner,entity3DManager);
};


void AttackController::Update(float dt) {
	// ヒットカウンターの更新
	hitCounter_.Update(dt);
	// ヒットボックスシステムの更新
	hitBoxSystem_->Update(dt);
	if (IsAttack()) {
		// コンボシステムの更新
		comboSystem_->UpdateCombo(dt);
	}
};
