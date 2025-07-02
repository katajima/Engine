#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Player.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void GameUI::Initialize(Entity2DManager* entity2DManager)
{
	entity2DManager_ = entity2DManager;


	float xpos = 1050;
	Vector2 scale{ 75,75 };
	icon_B = std::make_unique<Sprite>();
	InitSprite(icon_B.get(), "resources/Texture/icon/B.png", {xpos ,500}, scale);


	icon_Y = std::make_unique<Sprite>();
	InitSprite(icon_Y.get(), "resources/Texture/icon/Y.png", { xpos ,550 }, scale);


	icon_X = std::make_unique<Sprite>();
	InitSprite(icon_X.get(), "resources/Texture/icon/X.png", { xpos ,600 }, scale);


	icon_RT = std::make_unique<Sprite>();
	InitSprite(icon_RT.get(), "resources/Texture/icon/X.png", { xpos ,450 }, scale);

	scale = { 100,33 };
	xpos = 1120;

	text_normal = std::make_unique<Sprite>();
	InitSprite(text_normal.get(), "resources/Texture/text/normalAttack.png", { xpos ,520 }, scale);

	text_jump = std::make_unique<Sprite>();
	InitSprite(text_jump.get(), "resources/Texture/text/JumpAttack.png", { xpos ,570 }, scale);

	text_dash = std::make_unique<Sprite>();
	InitSprite(text_dash.get(), "resources/Texture/text/DashAttack.png", { xpos ,620 }, scale);

	text_special = std::make_unique<Sprite>();
	InitSprite(text_special.get(), "resources/Texture/text/special.png", { xpos ,470 }, scale);

	scale = { 100 * 1.5f,33 * 1.5f };

	text_hit = std::make_unique<Sprite>();
	InitSprite(text_hit.get(), "resources/Texture/text/Hit.png", { 1100,110 }, scale);

	text_clera = std::make_unique<Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.png", { 300,200 }, scale);
	text_clera->SetSize(2);
	text_clera->SetColor({ 0,1,0,1 });


	text_over = std::make_unique<Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.png", { 300,200 }, scale);
	text_over->SetSize(2);
	text_over->SetColor({ 1,0,0,1 });

	xpos = { 950 };
	numpos[2] = { xpos,100 };
	numpos[1] = { xpos + (50 * 1),100 };
	numpos[0] = { xpos + (50 * 2),100 };


	for (int i = 0; i < 3; i++) {
		auto sprite = std::make_unique<Sprite>();
		InitSprite(sprite.get(), "resources/Texture/num/Number_x64y96.png", numpos[i], { 64 * 2 / 3, 96 * 2 / 3 });
		sprite->SetTextureSize({ 64,96 });
		sprite_.push_back(std::move(sprite));
	}

}

void GameUI::Update()
{
}

void GameUI::Draw()
{
	icon_B->Update();
	//icon_Y->Update();
	//icon_X->Update();
	icon_RT->Update();
	text_normal->Update();
	//text_jump->Update();
	//text_dash->Update();
	text_special->Update();
	text_hit->Update();
	icon_B->Draw();
	//icon_Y->Draw();
	//icon_X->Draw();
	//if (player_->GetIsSpecial()) {
		icon_RT->Draw();
		text_special->Draw();
	//}
	text_normal->Draw();
	//text_jump->Draw();
	//text_dash->Draw();
	text_hit->Draw();


	int adsbhads = 0;// player_->GetHitCount();
	if (adsbhads >= 999) {
		adsbhads = 999;
	}

	int numDigits = (adsbhads == 0) ? 1 : static_cast<int>(log10(adsbhads)) + 1;


	Vector2 texSize = { 64, 96 };

	for (int i = 0; i < sprite_.size(); i++) {
		int digit = (static_cast<int>(adsbhads) / static_cast<int>(pow(10, i))) % 10;
		sprite_[i]->SetTextureLeftTop(Vector2((texSize.x * digit), 0));


		sprite_[i]->Update();
		sprite_[i]->Draw();
	}
}

void GameUI::SetPlayer(BasePlayer* player)
{
	player_ = player;
}