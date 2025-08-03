#pragma once
#include "DirectXGame/application/base/BaseClass/UI/BaseUI.h"

class BasePlayer;
class GameUI : public BaseUI
{
public:
	void Initialize(Input* input,Entity2DManager* entity2DManager, GlobalVariables* globalVariables) override;

	void Update() override;


	void Draw() override;

	void SetPlayer(BasePlayer* player);

private:
	std::unique_ptr<Sprite> text_clera;
	std::unique_ptr<Sprite> text_over;
private:
	BasePlayer* player_;

};

