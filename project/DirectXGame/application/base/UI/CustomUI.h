#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

/// <summary>
/// CustomUIを管理・実装するクラス。
/// </summary>
class CustomUI : public BaseUI {
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

};

