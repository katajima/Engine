#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"
#include "DirectXGame/application/base/Game/Base/GameData.h"

// 前方宣言
namespace Character {
	class BasePlayer;
}

/// <summary>
/// タイトルUI
/// </summary>
class TitleUI : public BaseUI {
public:

	//初期化
	void Initialize(InputSystem* inputSystem, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

	// ゲームデータ取得
	void SetGamePlayData(const Game::GameData data) { data_ = data; };
private:
	// ボタンUI処理
	void ButtonUiProcess(float dt);
private:
	Game::GameData data_;
private:
	std::unique_ptr<Engine::Sprite> iconB_;
	std::unique_ptr<Engine::Sprite> title_;
private:
	// ボタン色
	Color buttonColor_{1,1,1,1};
	float buttonTimer_ = 0.0f;
	int buttonClock_ = 1;
};

