#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

class CustomUI : public BaseUI {
public:
	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	// 更新
	void Update(float dt) override;

	// 描画
	void Draw() override;



private:

};

