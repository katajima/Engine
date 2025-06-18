#pragma once

#include "DirectXGame/application/base/Attack/Type/BaseAttack.h"
#include "DirectXGame/application/base/Attack/Type/BaseAttackType.h"


class BaseAttackFactory {
public:
	virtual ~BaseAttackFactory() = default;

	// attackId から派生クラスを生成
	virtual std::unique_ptr<BaseAttack> Create(const std::string& attackId) = 0;
};