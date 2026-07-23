#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
namespace Character {
	class ParameterComponent;
}

/// <summary>
/// リザルトUIクラス
/// </summary>
class ResultUI : public BaseUI {
public:
	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables) override;

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
	// タイトルに遷移するUI
	std::unique_ptr<Engine::Sprite> titleUI_;
	std::unique_ptr<Engine::Sprite> titleUItxt_;
	// インゲームに遷移するUI
	std::unique_ptr<Engine::Sprite> gamePlayUI_;
	std::unique_ptr<Engine::Sprite> gamePlayUItxt_;
private:
	int selectedIndex = 0;	// 選択されているインデックス
private:
	Vector2 uiSize_ = { 256,64 };
	Vector2 uiAnchorPoint_ = { 0.5f,0.5f };
	Color uiColor_ = { 0.5f,0.5f ,0.5f ,1.0f };
	Color uiSelectColor_ = { 1.0f,1.0f ,1.0f ,1.0f };
	Color preparationColor_ = { 0.15f,0.15f ,0.15f ,1.0f };
private:
	Vector2 titlePos_{900,650};
	Vector2 gamePlayPos_{ 300,650 };
};