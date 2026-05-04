#pragma once
#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
namespace Character {
	class ParameterComponent;
}

/// <summary>
/// リザルトUIクラス
/// </summary>
class ResultScoreUI : public BaseUI {
public:
	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables) override;

	// 更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

	// スコア設定
	void SetScore(float score) { this->score = score; };

private:

private:
	Vector2 uiPos = { 600,300 };
	Vector2 uiSize_ = { 64,96 };
	Vector2 offset = { 20,0.0f };
	Vector2 uiAnchorPoint_ = { 0.5f,0.5f };
	Color uiColor_ = { 0.5f,0.5f ,0.5f ,1.0f };
	Vector2 uiNameSize = { 256,64 };

private:
	float score = 0.0f;

};