#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"
#include "DirectXGame/application/base/Game/Base/GameData.h"


// 前方宣言
namespace Character {
	class BasePlayer;
}

/// <summary>
/// ゲーム開始UI
/// </summary>
class GameStartUI : public BaseUI {
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
	/// フェード開始
	/// </summary>
	void IsFade() { isfade_ = true; }
	/// <summary>
	/// フェード
	/// </summary>
	void IsNofade() { isfade_ = false; }

	void SetNum(float num) { this->num = num; };
	//
	void IsNum(bool is) { isNum = is; }
private:
	// フェード
	std::unique_ptr<Engine::Sprite> fade_ = nullptr;
	// ゲーム開始準備UI
	std::unique_ptr<Engine::Sprite> gameStartPreparationUI_ = nullptr;

private:
	// 位置
	Vector2 position_ = { 640, 360 };
	Vector2 uiSize_ = { 1280,720 };
	Vector2 uiAnchorPoint_ = { 0.5f,0.5f };
	Color uiColor_ = { 1.0f,1.0f ,1.0f ,0.0f };
	bool isfade_ = false;
private:
	Vector2 gameStartPosition_ = { 1150, 120 };
	Vector2 gameStartUiSize_ = { 256,64 };
private:
	Vector2 numPosition_ = { 640, 360 };
	Vector2 numUiSize_ = { 150,300 };
	Vector2 numUiAnchorPoint_ = { 0.5f,0.5f };
	Color numUiColor_ = { 0.9f,0.9f ,0.9f ,1.0f };
	bool isNum = false;
	float num = 0;
};