#pragma once
#include "DirectXGame/engine/UI/UIBoard.h"

#include "DirectXGame/application/base/Input/InputSystem.h"

// 前方宣言
namespace Engine {
	class EntityManager;
	class GlobalVariables;
}

/// <summary>
/// UI基底クラス
/// </summary>
class BaseUI
{
public:
	//初期化
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) = 0;

	// 毎フレーム更新
	virtual void Update(float dt) = 0;

	// 描画
	virtual void Draw() = 0;

	// 画面比率設定
	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;	// 画面左上座標
		ratio_ = ratio;				// 画面比率
	};


protected:
	// スプライト初期化
	void InitSprite(Engine::Sprite* sprite,std::string texFile,Vector2 pos,Vector2 size);
	// チェックボックス初期化
	void InitUICheckBox(std::string name,Vector2 pos);
	// スライダー初期化
	void InitUISlider(std::string name, Vector2 pos);
	// メーター初期化
	void InitUIMeter(std::string name, Vector2 pos, bool useSprite = false);
	// ペア初期化
	void InitUIPair(std::string name, Vector2 pos);
	// カウンター初期化
	void InitUICount(std::string name, Vector2 pos,int instance = 1,bool useSprite = false);


	// 更新
	void UpdateUIElement(float deltaTime);
	// 描画
	void DrawUIElement();
	
	// チェックボックス取得
	Engine::UICheckBox* GetUICheckBox(std::string name);
	// スライダー取得
	Engine::UISlider* GetUISlider(std::string name);
	// メータ取得
	Engine::UIMeter* GetUIMeter(std::string name);
	// ペア取得
	Engine::UIPair* GetUIPair(std::string name);
	// カウンター取得
	Engine::UICount* GetUICount(std::string name);

protected:
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	InputSystem* inputSystem = nullptr;


	std::unique_ptr <Engine::UIBaseBoard> board_;


	std::map<std::string,std::unique_ptr<Engine::UICheckBox>> uiCheckBox_;
	std::map<std::string,std::unique_ptr<Engine::UISlider>> uiSlider_;
	std::map<std::string,std::unique_ptr<Engine::UIMeter>> uiMeter_;
	std::map<std::string,std::unique_ptr<Engine::UIPair>> uiPair_;
	std::map<std::string,std::unique_ptr<Engine::UICount>> uiCount_;

	


	Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
	Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率



	std::vector<std::unique_ptr<Engine::Sprite>> sprite_;
private:
};

