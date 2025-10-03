#pragma once
#include "DirectXGame/engine/UI/UIBoard.h"
class Entity2DManager;
class GlobalVariables;
class BaseUI
{
public:
	virtual void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;

	void SetImageLeftTopPosAndRatio(Vector2 leftTopPos, Vector2 ratio) {
		leftTopPos_ = leftTopPos;
		ratio_ = ratio;
	};


protected:
	void InitSprite(Sprite* sprite,std::string texFile,Vector2 pos,Vector2 size);

	void InitUICheckBox(std::string name,Vector2 pos);
	void InitUISlider(std::string name, Vector2 pos);
	void InitUIMeter(std::string name, Vector2 pos, bool useSprite = false);
	void InitUIPair(std::string name, Vector2 pos);
	void InitUICount(std::string name, Vector2 pos,int instance = 1,bool useSprite = false);



	void UpdateUIElement(float deltaTime);
	void DrawUIElement();
	
	UICheckBox* GetUICheckBox(std::string name);
	UISlider* GetUISlider(std::string name);
	UIMeter* GetUIMeter(std::string name);
	UIPair* GetUIPair(std::string name);
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

