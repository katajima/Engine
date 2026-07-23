#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"


/// <summary>
/// セレクトシーンUI
/// </summary>
class SelectUI :public BaseUI {
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
	/// 選択されているインデックスの設定
	/// </summary>
	void SetSelectedIndex(int index) { selectedIndex = index; };

private:
	// メインゲームに遷移するUI
	std::unique_ptr<Engine::Sprite> mainGameUI_;
	std::unique_ptr<Engine::Sprite> mainGameUItxt_;
	// カスタムゲームに遷移するUI
	std::unique_ptr<Engine::Sprite> customUI_;
	// タイトルに遷移するUI
	std::unique_ptr<Engine::Sprite> titleUI_;
	std::unique_ptr<Engine::Sprite> titleUItxt_;
	// 操作説明UI
	std::unique_ptr<Engine::Sprite> operationUI_;

	// カスタムゲームに遷移するUI
	std::unique_ptr<Engine::Sprite> preparationUI01_;
	// 操作説明UI
	std::unique_ptr<Engine::Sprite> preparationUI03_;
private:
	int selectedIndex = 0;	// 選択されているインデックス
private:
	Vector2 uiSize_ = { 256,64 };
	Vector2 uiAnchorPoint_ = { 0.5f,0.5f };
	Color uiColor_ = { 0.5f,0.5f ,0.5f ,1.0f };
	Color uiSelectColor_ = { 1.0f,1.0f ,1.0f ,1.0f };
	Color preparationColor_ = { 0.15f,0.15f ,0.15f ,1.0f };
private:
	Vector2 mainGameUIPos_ = { 100,120 };
	Vector2 customGameUIPos_ = { 100,220 };
	Vector2 titleUIPos_ = { 100,420 };
	Vector2 operationUIPos_ = { 100,320 };
};