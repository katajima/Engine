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
        
        ImGui::Begin("Uppe");
        ImGui::InputFloat("timer", &timer_);
        ImGui::InputFloat3("timer", &transforms_["Player"]->translate_.x);
        ImGui::Checkbox("isFinished", &isFinished_);
        ImGui::End();


        if (timer_ <= GetStartupAndActiveFrame() && timer_ >= GetStartupFrame()) {
             transforms_["Player"]->translate_.x += 0.5f;
        }
        else if (timer_ <= GetStartupAndActiveFrame()) {
            transforms_["Player"]->translate_.x += 0.1f;
        }
        else {
            transforms_["Player"]->translate_.x += 0.1f;
        }
       

       

    }

private:
    float upwardForce_ = 0.0f;  // 上昇力
    
};
