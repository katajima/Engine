#include "AttackController.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void AttackController::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
	Character::ParameterComponent* base, Character::BaseCharacter* owner) {
	this->globalVariables = globalVariables;

	// ダメージ計算用ステータスの初期化
	combatStat_ = std::make_unique<CombatStat>();
	combatStat_->Initialize(base);

	// コンボシステムの初期化
	comboSystem_ = std::make_unique<Combo::System>();
	comboSystem_->Initialize(owner, entityManager->Get3DLineCommon(), globalVariables);

	// ロックオンシステム初期化
	lockOnSystem_ = std::make_unique<LockOnSystem>();
	lockOnSystem_->Initialize(owner);

};


void AttackController::Update(const Character::CharacterContext& ctx) {
	// ヒットカウンターの更新
	if (!isStopHitTimer_) {
		hitCounter_.Update(ctx.dt);
	}

	if (IsAttack() || isDebugEditor_) {
		// コンボシステムの更新
		comboSystem_->Update(ctx);
	}
};
