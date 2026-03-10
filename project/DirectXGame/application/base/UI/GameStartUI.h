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
	//初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

	// フェード開始
	void IsFade() { isfade_ = true; }
	// フェード
	void IsNofade() { isfade_ = false; }
private:
	// フェード
	std::unique_ptr<Engine::Sprite> fade_;
private:
	// 位置
	Vector2 position_ = { 640, 360 };
	Vector2 uiSize_ = { 1280,720 };
	Vector2 uiAnchorPoint_ = { 0.5f,0.5f };
	Color uiColor_ = { 1.0f,1.0f ,1.0f ,0.0f };
	bool isfade_ = false;
};