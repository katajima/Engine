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
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

	// ゲームデータ取得
	void SetGamePlayData(const Game::GameData data) { data_ = data; };

	// 動く
	void Action() { isAction_ = true; }

private:
	// ボタンUI処理
	void ButtonUiProcess(float dt);
	// タイトルUI
	void TitleUiProcess(float dt);

private:
	Game::GameData data_;
private:
	// タイトルスプライト
	std::unique_ptr<Engine::Sprite> title_;
	//
	Vector2 titlePos_ = { 640,200 };
private:
	// ボタンスプライト
	std::unique_ptr<Engine::Sprite> iconB_;
	// ボタン色
	Color buttonColor_{1,1,1,1};
	int buttonClock_ = 1;
private:
	// 
	bool isAction_ = false;

};

