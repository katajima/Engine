#pragma once
#include "DirectXGame/engine/UI/BaseUI.h"
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

	/// <summary>
	/// ゲームデータ取得
	/// </summary>
	void SetGamePlayData(const Game::GameData data) { data_ = data; };

	/// <summary>
	/// 動く
	/// </summary>
	void Action() { isAction_ = true; }

private:
	/// <summary>
	/// ボタンUI処理
	/// </summary>
	void ButtonUiProcess(float dt);
	/// <summary>
	/// タイトルUI
	/// </summary>
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

