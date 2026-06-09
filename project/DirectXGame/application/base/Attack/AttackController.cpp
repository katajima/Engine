#include "AttackController.h"
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void AttackController::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
	Character::ParameterComponent* base, Character::BaseCharacter* owner) {
	// 攻撃データやデバッグ調整に使うグローバル変数を保持する
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
		// ヒット間隔の計測を進め、連続ヒット表示や判定に使う
		hitCounter_.Update(ctx.dt);
	}

	if (IsAttack() || isDebugEditor_) {
		// コンボシステムの更新
		// デバッグエディタ中は攻撃中でなくてもコンボ調整を動かす
		comboSystem_->Update(ctx);
	}
};
