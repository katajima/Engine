#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void GameUI::Initialize(Input* input,Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;
	float xpos = 1050;
	Vector2 scale{ 75,75 };
	

	board_ = std::make_unique<UIBaseBoard>();
	board_->Init(input_,entity2DManager_, "bord", { 100,100 }, { 400,400 });

	InitUIPair("normalAttack", { 1120,520 });
	UIPair* normalAttackPair = GetUIPair("normalAttack");
	normalAttackPair->SetOffset(10.0f);
	normalAttackPair->SetUIPairDrectionType(UIPairDrectionType::Right);
	normalAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/icon/B.png");
	normalAttackPair->GetFirstSprite()->SetAnchorPoint({0.5f,0.5f});
	normalAttackPair->GetFirstSprite()->SetSize({75.0f,75.0f });
	normalAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/text/normalAttack.png");
	normalAttackPair->GetSecondSprite()->SetAnchorPoint({ 0.0f,0.5f });
	normalAttackPair->GetSecondSprite()->SetSize({ 100,33 });
	InitUIPair("special", { 1120,570 });
	UIPair* specialPair = GetUIPair("special");
	specialPair->SetOffset(10.0f);
	specialPair->SetUIPairDrectionType(UIPairDrectionType::Right);
	specialPair->GetFirstSprite()->SetTextureName("resources/Texture/icon/RB.png");
	specialPair->GetFirstSprite()->SetAnchorPoint({ 0.5f,0.5f });
	specialPair->GetFirstSprite()->SetSize({ 75.0f,75.0f });
	specialPair->GetSecondSprite()->SetTextureName("resources/Texture/text/special.png");
	specialPair->GetSecondSprite()->SetAnchorPoint({ 0.0f,0.5f });
	specialPair->GetSecondSprite()->SetSize({ 100,33 });

	xpos = 1120;
	scale = { 100 * 1.5f,33 * 1.5f };

	text_clera = std::make_unique<Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.png", { 300,200 }, scale);
	text_clera->SetSize(2);
	text_clera->SetColor({ 0,1,0,1 });


	text_over = std::make_unique<Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.png", { 300,200 }, scale);
	text_over->SetSize(2);
	text_over->SetColor({ 1,0,0,1 });


	InitUICount("hitCount", { 950,100 }, 3,true);
	UICount* hitCount = GetUICount("hitCount");
	hitCount->SetMaxSize({ 64 * 2 / 3, 96 * 2 / 3 }, { 20.0f,0.0f });
	hitCount->SetTextuerSize({ 64,96 });
	hitCount->SetCountMax(999);
	hitCount->GetNameSprite()->SetTextureName("resources/Texture/text/Hit.png");
	hitCount->GetNameSprite()->SetSize({ 100 * 1.5f,33 * 1.5f });


	board_->CreateUIElement(UIType::CheckBox, "botton",{100,100});
	board_->CreateUIElement(UIType::UISlider, "slider",{100,100});

}

void GameUI::Update()
{
	//UIButton* botton = board_->GetUIElement<UIButton>(UIType::CheckBox, "botton");
	board_->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);

	UISlider* slider = board_->GetUIElement<UISlider>(UIType::UISlider, "slider");
	slider->SetPos({30,50});

	board_->Update(0);
	
	

	UICount* hitCount = GetUICount("hitCount");
	hitCount->SetCount(static_cast<float>(player_->GetHitCount()));

	UpdateUIElement(0);
}

void GameUI::Draw()
{
	board_->Draw();

	DrawUIElement();
}

void GameUI::SetPlayer(BasePlayer* player)
{
	player_ = player;
}