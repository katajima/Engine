#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
namespace Engine {
	class Entity2DManager;
}


class FpsUI : public BaseUI {
public:

	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables) override;

	// 更新
	void Update(float dt) override;

	// 描画
	void Draw() override;


private:
	float nowTime = 0.0f;
	float fps = 0.0f;
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
};

