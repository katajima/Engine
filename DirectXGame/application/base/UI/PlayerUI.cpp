#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void PlayerUI::Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;


	InitUIMeter("HPBer",{30,650}, true);
	InitUIMeter("SpecailBer",{30,700}, true);


	UIMeter* hpber  = GetUIMeter("HPBer");
	hpber->SetMaxSize({ 100,30 },{4.0f,4.0f });
	hpber->SetMeterMinMax(0.0f,100.0f);
	hpber->GetMeterSprite()->SetColor({ 0,1,0,1 });
	hpber->SetMeterType(UIMeterType::Left);
	hpber->GetNameSprite()->SetTextureName("resources/Texture/text/HP.png");
	hpber->GetNameSprite()->SetSize({60,40});

	UIMeter* specailBer_ = GetUIMeter("SpecailBer");
	specailBer_->SetMaxSize({ 100,30 },{4.0f,4.0f });
	specailBer_->SetMeterMinMax(0.0f, 20.0f);
	specailBer_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	specailBer_->SetMeterType(UIMeterType::Left);
	specailBer_->GetNameSprite()->SetTextureName("resources/Texture/text/SP.png");
	specailBer_->GetNameSprite()->SetSize({ 60,40 });



	textMax_ = std::make_unique<Sprite>();
	textMax_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/text/max.png");
	textMax_->SetColor({ 1,0,0,1 });
	textMax_->SetPosition({ 30,700 });
	textMax_->SetRotation(DegreesToRadians(-30));
	textMax_->SetAnchorPoint({ 0.5f,0.5f });
	textMax_->SetSize(0.25f);

	textRB_ = std::make_unique<Sprite>();
	textRB_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/icon/RB.png");
	textRB_->SetColor({ 1,1,1,1 });
	textRB_->SetPosition({ 1280 / 2,550 });
	textRB_->SetAnchorPoint({ 0.5f,0.5f });
	textRB_->SetSize(0.2f);
}

void PlayerUI::Update()
{
	UIMeter* hpber = GetUIMeter("HPBer");
	UIMeter* specailBer_ = GetUIMeter("SpecailBer");
	hpber->SetMeter(sizeHP_);
	specailBer_->SetMeter(sizeSpecialGauge_);

	UpdateUIElement(0);
}

void PlayerUI::Draw()
{
	DrawUIElement();
	


	if (isTextMax_) {
		textMax_->Update();
		textMax_->Draw();
	};

	if (isTextRB_) {
		textRB_->Update();
		textRB_->Draw();
	}
}
