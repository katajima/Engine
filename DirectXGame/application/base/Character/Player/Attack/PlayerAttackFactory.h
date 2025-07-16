#pragma once

#include "DirectXGame/application/base/Attack/Factory/BaseAttackFactory.h"
#include "DirectXGame/application/base/Character/Player/Attack/PlayerAttackType.h"


class PlayerAttackFactory : public BaseAttackFactory {
public:
    std::unique_ptr<BaseAttack> Create(const std::string& attackId) override {
        if (attackId == "Punch1") return std::make_unique<UppercutAttack>();
        if (attackId == "Punch2") return std::make_unique<UppercutAttack>();
        return nullptr;
    }
};