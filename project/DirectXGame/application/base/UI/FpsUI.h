#pragma once
#include "DirectXGame/engine/UI/BaseUI.h"

/// <summary>
/// FpsUIを管理・実装するクラス。
/// </summary>
class FpsUI : public BaseUI {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;


private:
	float nowTime = 0.0f;
	float fps = 0.0f;
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
};

