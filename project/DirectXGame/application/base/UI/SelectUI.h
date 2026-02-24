#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"


/// <summary>
/// セレクトシーンUI
/// </summary>
class SelectUI :public BaseUI {
public:

	//初期化
	void Initialize(InputSystem* inputSystem, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

private:
	std::unique_ptr<Engine::Sprite> text_clera;
	std::unique_ptr<Engine::Sprite> text_over;
private:

};