#pragma once
#include "BaseUI.h"

class Player;
class GameUI : public BaseUI
{
public:
	void Initialize(Entity2DManager* entity2DManager) override;

	void Update() override;


	void Draw() override;

	void SetPlayer(Player* player);

private:
	std::unique_ptr<Sprite> icon_X;
	std::unique_ptr<Sprite> icon_Y;
	std::unique_ptr<Sprite> icon_B;
	std::unique_ptr<Sprite> icon_RT;
	std::unique_ptr<Sprite> text_jump;
	std::unique_ptr<Sprite> text_normal;
	std::unique_ptr<Sprite> text_dash;
	std::unique_ptr<Sprite> text_special;
	std::unique_ptr<Sprite> text_hit;
	std::unique_ptr<Sprite> text_clera;
	std::unique_ptr<Sprite> text_over;



private:
	Player* player_;

};

