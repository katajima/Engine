#include "BossEnemyState.h"

BossEnemyStateMove::BossEnemyStateMove(BaseEnemy* enemy)
	: BaseEnemyState("Move", enemy){
}
// 更新
void BossEnemyStateMove::Update() {
};

// 終了
void BossEnemyStateMove::Exit() {
};

// 初期化
void BossEnemyStateMove::Enter() {
};


BossEnemyStateAttack::BossEnemyStateAttack(BaseEnemy* enemy)
	: BaseEnemyState("Attack", enemy) {
}

// 更新
void BossEnemyStateAttack::Update() {
};

// 終了
void BossEnemyStateAttack::Exit() {
};

// 初期化
void BossEnemyStateAttack::Enter() {
};

BossEnemyStateSpecial::BossEnemyStateSpecial(BaseEnemy* enemy)
	: BaseEnemyState("Special", enemy) {
}

// 更新
void BossEnemyStateSpecial::Update() {
};

// 終了
void BossEnemyStateSpecial::Exit() {
};

// 初期化
void BossEnemyStateSpecial::Enter() {
};

BossEnemyStateDie::BossEnemyStateDie(BaseEnemy* enemy)
	: BaseEnemyState("Die", enemy) {
}

// 更新
void BossEnemyStateDie::Update() {
};

// 終了
void BossEnemyStateDie::Exit() {
};

// 初期化
void BossEnemyStateDie::Enter() {
};