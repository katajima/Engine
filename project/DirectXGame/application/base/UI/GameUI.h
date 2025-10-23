#pragma once
#include "DirectXGame/application/base/BaseClass/UI/BaseUI.h"

// 前方宣言
class BasePlayer;

/// <summary>
/// ゲームUIクラス
/// </summary>
class GameUI : public BaseUI
{
public:
	//初期化
	void Initialize(Input* input,Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	//更新
	void Update() override;

	// 描画
	void Draw() override;

	// プレイヤ設定
	void SetPlayer(BasePlayer* player);

private:
	std::unique_ptr<Sprite> text_clera;
	std::unique_ptr<Sprite> text_over;
private:
	BasePlayer* player_;

};

