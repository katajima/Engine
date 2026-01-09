#include "NormalEnemySubState.h"
#include "NormalEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"


void NormalEnemyAttackReadySubState::Update(float deltaTime) {
    timer_ += deltaTime;
    BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
    // 後ろに後退

    enemy->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = -30.0f;
    enemy->DirectionMoveVelocity(-30.0f);
    if (timer_ > readyTime_) {
        // 攻撃へ遷移
        fsm_->ChangeState(AttackSubState::Swing);
    }
}


void NormalEnemyAttackSwingSubState::Enter() {
    BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
    timer_ = 0.0f;

    enemy->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 40.0f;
    enemy->DirectionMoveVelocity(40.0f);
    dire_ = enemy->TargetDirection();
}

void NormalEnemyAttackSwingSubState::Update(float deltaTime) {
    BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
    timer_ += deltaTime;

    // 前進

    enemy->GetMoveComponent()->GetMoveSystem()->GetData().maxSpeed = 40.0f;
    enemy->Velocity() = dire_ * 40.0f;

    if (timer_ > swingTime_) {
        fsm_->ChangeState(AttackSubState::End);
    }
}

void NormalEnemyAttackSwingSubState::Exit() {
}


void NormalEnemyAttackEndSubState::Enter() {
    timer_ = 0.0f;
    rotate_ = character_->GetObjectComponent()->GetWorldTransform().rotate_;

    // プレイヤー方向を向くための目標角度を計算
    BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
    Vector3 playerPos = enemy->GetTargetPos();
    Vector3 toPlayer = Subtract(playerPos, enemy->GetWorldTransform().translate_);
    toPlayer.y = 0.0f; // 水平面のみ考慮

    // atan2(x, z) でY軸周り角度
    targetRotateY_ = std::atan2(toPlayer.x, toPlayer.z);
}

void NormalEnemyAttackEndSubState::Update(float deltaTime) {
    BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
    Vector3 playerPos = enemy->GetTargetPos();

    timer_ += deltaTime;

    // 角度補間
    float t = std::min(timer_ / endTime_, 1.0f); // 0～1 の範囲
    float currentY = character_->GetObjectComponent()->GetWorldTransform().rotate_.y;

    // 角度の最短差分を考慮（0～π方向への補間）
    float diff = targetRotateY_ - currentY;
    diff = std::atan2(std::sin(diff), std::cos(diff)); // -π ～ π に正規化

    float newY = currentY + diff * t * 0.2f; // 0.2f は回転速度係数（好みで調整）

    character_->GetObjectComponent()->GetWorldTransform().rotate_.y = newY;

    // 終了判定
    if (timer_ > endTime_) {
        fsm_->SetFinished(true);
    }
}