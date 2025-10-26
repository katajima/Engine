#pragma once
#include "DirectXGame/engine/UI/UIBoard.h"


// 前方宣言
class Entity2DManager;
class GlobalVariables;


/// <summary>
/// UI基底クラス
/// </summary>
class BaseUI
{
public:
	//初期化
	virtual void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;

	// 画面比率設定
	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;
		ratio_ = ratio;
	};


protected:
	// スプライト初期化
	void InitSprite(Sprite* sprite,std::string texFile,Vector2 pos,Vector2 size);
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
	UICheckBox* GetUICheckBox(std::string name);
	// スライダー取得
	UISlider* GetUISlider(std::string name);
	// メータ取得
	UIMeter* GetUIMeter(std::string name);
	// ペア取得
	UIPair* GetUIPair(std::string name);
	// カウンター取得
	UICount* GetUICount(std::string name);

protected:
	Entity2DManager* entity2DManager_;
	GlobalVariables* globalVariables_;
	Input* input_;


	std::unique_ptr <UIBaseBoard> board_;


	std::map<std::string,std::unique_ptr<UICheckBox>> uiCheckBox_;
	std::map<std::string,std::unique_ptr<UISlider>> uiSlider_;
	std::map<std::string,std::unique_ptr<UIMeter>> uiMeter_;
	std::map<std::string,std::unique_ptr<UIPair>> uiPair_;
	std::map<std::string,std::unique_ptr<UICount>> uiCount_;

	


	Vector2 leftTopPos_{};	// 画面左上位置(ImGuiImage)
	Vector2 ratio_{};		// 画面サイズとImGuiImageにした比率



	std::vector<std::unique_ptr<Sprite>> sprite_;
private:
};

