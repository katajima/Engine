#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

class FpsUI : public BaseUI {
public:

	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	// 更新
	void Update(float dt) override;

	// 描画
	void Draw() override;


private:
	float nowTime = 0.0f;
	float fps = 0.0f;
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
};

