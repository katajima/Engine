#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"

class AttackComponent {
public:
    void Initialize(BaseCharacter* owner);
    void Update(float deltaTime);
    //void OnHit(BaseCharacter* target);
    

protected:
    BaseCharacter* owner_ = nullptr;
    
};
