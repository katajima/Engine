#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"


/// <summary>
/// セレクトシーンUI
/// </summary>
class SelectUI :public BaseUI {
public:

	//初期化
	void Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	//更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	std::unique_ptr<Sprite> text_clera;
	std::unique_ptr<Sprite> text_over;
private:

};