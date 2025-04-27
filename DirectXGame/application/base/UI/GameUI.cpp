#include "GameUI.h"
#include "DirectXGame/application/base/Player/Player.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void GameUI::Initialize(Entity2DManager* entity2DManager)
{
	entity2DManager_ = entity2DManager;


	float xpos = 1050;
	Vector2 scale{ 75,75 };
	icon_B = std::make_unique<Sprite>();
	InitSprite(icon_B.get(), "resources/Texture/icon/B.png", {xpos ,500}, scale);


	icon_Y = std::make_unique<Sprite>();
	icon_Y->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/Y.png");
	icon_Y->SetPosition({ xpos,550 });
	icon_Y->SetSize(scale);

	icon_X = std::make_unique<Sprite>();
	icon_X->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/X.png");
	icon_X->SetPosition({ xpos,600 });
	icon_X->SetSize(scale);

	icon_RT = std::make_unique<Sprite>();
	icon_RT->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/RB.png");
	icon_RT->SetPosition({ xpos,450 });
	icon_RT->SetSize(scale);

	scale = { 100,33 };
	xpos = 1120;

	text_normal = std::make_unique<Sprite>();
	text_normal->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/normalAttack.png");
	text_normal->SetPosition({ xpos,520 });
	text_normal->SetSize(scale);

	text_jump = std::make_unique<Sprite>();
	text_jump->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/JumpAttack.png");
	text_jump->SetPosition({ xpos,570 });
	text_jump->SetSize(scale);

	text_dash = std::make_unique<Sprite>();
	text_dash->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/DashAttack.png");
	text_dash->SetPosition({ xpos,620 });
	text_dash->SetSize(scale);

	text_special = std::make_unique<Sprite>();
	text_special->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/special.png");
	text_special->SetPosition({ xpos,470 });
	text_special->SetSize(scale);

	scale = { 100 * 1.5f,33 * 1.5f };

	text_hit = std::make_unique<Sprite>();
	text_hit->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/Hit.png");
	text_hit->SetPosition({ 1100,110 });
	text_hit->SetSize(scale);

	text_clera = std::make_unique<Sprite>();
	text_clera->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/clear.png");
	text_clera->SetPosition({ 300,200 });
	//text_clera->SetAnchorPoint({ 0.5f,0.5f });
	text_clera->SetSize(2);
	text_clera->SetColor({ 0,1,0,1 });
	text_over = std::make_unique<Sprite>();
	text_over->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/text/over.png");
	text_over->SetPosition({ 300,200 });
	//text_over->SetAnchorPoint({ 0.5f,0.5f });
	text_over->SetSize(2);
	text_over->SetColor({ 1,0,0,1 });
}

void GameUI::Update()
{
}

void GameUI::Draw()
{
	icon_B->Update();
	icon_Y->Update();
	icon_X->Update();
	icon_RT->Update();
	text_normal->Update();
	text_jump->Update();
	text_dash->Update();
	text_special->Update();
	text_hit->Update();
	icon_B->Draw();
	icon_Y->Draw();
	icon_X->Draw();
	if (player_->GetIsSpecial()) {
		icon_RT->Draw();
		text_special->Draw();
	}
	text_normal->Draw();
	text_jump->Draw();
	text_dash->Draw();
	text_hit->Draw();
}

void GameUI::SetPlayer(Player* player)
{
	player_ = player;
}