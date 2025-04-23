#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void PlayerUI::Initialize(Entity2DManager* entity2DManager)
{
	HpBer_ = std::make_unique<Sprite>();
	HpBer_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/Image.png");
	//HpBer_->SetSize({ 50,-float(hp) });
	HpBer_->SetColor({ 0,1,0,1 });
	HpBer_->SetPosition({ 100,650 });

	SpecailBer_ = std::make_unique<Sprite>();
	SpecailBer_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/Image.png");
	//SpecailBer_->SetSize({ 50,-float(specialAttack.specialGauge) });
	SpecailBer_->SetColor({ 0,0,1,1 });
	SpecailBer_->SetPosition({ 40,650 });

	textMax_ = std::make_unique<Sprite>();
	textMax_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/text/max.png");
	textMax_->SetColor({ 1,0,0,1 });
	textMax_->SetPosition({ 45,350 });
	textMax_->SetRotation(DegreesToRadians(-30));
	textMax_->SetAnchorPoint({ 0.5f,0.5f });
	textMax_->SetSize(0.25f);

	textRB_ = std::make_unique<Sprite>();
	textRB_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/icon/RB.png");
	textRB_->SetColor({ 1,1,1,1 });
	textRB_->SetPosition({ 1280 / 2,550 });
	textRB_->SetAnchorPoint({ 0.5f,0.5f });
	textRB_->SetSize(0.02f);
}

void PlayerUI::Update()
{
}

void PlayerUI::Draw()
{
	HpBer_->SetPosition({ 100,650 });
	HpBer_->SetSize({ 50,-sizeHP_* 2 });
	HpBer_->Update();
	HpBer_->Draw();

	SpecailBer_->SetSize({ 50,-sizeSpecialGauge_ * 2 * 2.5f });
	SpecailBer_->Update();
	SpecailBer_->Draw();

	if (isTextMax_) {


		textMax_->Update();
		textMax_->Draw();
	};

	if (isTextRB_) {
		textRB_->Update();
		textRB_->Draw();
	}
}
