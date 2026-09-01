#pragma once
#include "DirectXGame/engine/UI/BaseUI.h"
#include "DirectXGame/application/base/Game/Base/GameData.h"

// 前方宣言
namespace Character {
	class BasePlayer;
}

/// <summary>
/// ポーズUI
/// </summary>
class PoseUI : public BaseUI {
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

private:

};

