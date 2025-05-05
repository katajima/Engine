#pragma once

#include "DirectXGame/application/base/Attack/Type/BaseAttackType.h"


class UppercutAttack : public BlowAttack {
public:
    void Start() override {
        BlowAttack::Start();
        upwardForce_ = 5.0f;  // 上昇力（上向き攻撃）
      
    }

    void Update(float dt) override {
        BlowAttack::Update(dt);
        // 上昇力を使った演出等


        transforms_["Player"]->translate_.x += 1;

    }

private:
    float upwardForce_ = 0.0f;  // 上昇力
    
};
