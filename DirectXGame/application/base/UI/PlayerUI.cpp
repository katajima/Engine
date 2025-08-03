#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void PlayerUI::Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;


	//InitUIMeter("HPBer",{30,650}, true);
	//InitUIMeter("SpecailBer",{30,700}, true);


	/*UIMeter* hpber  = GetUIMeter("HPBer");
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
	specailBer_->GetNameSprite()->SetSize({ 60,40 });*/



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

	InitUICheckBox("check1", { 300,100 });
	InitUICheckBox("check2", { 340,100 });
	InitUICheckBox("check3", { 380,100 });
	InitUICheckBox("check4", { 420,100 });
	InitUICheckBox("check5", { 460,100 });


	InitUIMeter("HPBer1", { 30,150 });
	UIMeter* HPBer1_ = GetUIMeter("HPBer1");
	HPBer1_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer1_->SetMeterMinMax(0.0f, 20.0f);
	HPBer1_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer1_->SetMeterType(UIMeterType::Left);
	InitUIMeter("HPBer2", { 30,250 });
	UIMeter* HPBer2_ = GetUIMeter("HPBer2");
	HPBer2_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer2_->SetMeterMinMax(0.0f, 20.0f);
	HPBer2_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer2_->SetMeterType(UIMeterType::Right);

	InitUIMeter("HPBer3", { 30,350 });
	UIMeter* HPBer3_ = GetUIMeter("HPBer3");
	HPBer3_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer3_->SetMeterMinMax(0.0f, 20.0f);
	HPBer3_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer3_->SetMeterType(UIMeterType::Top);

	InitUIMeter("HPBer4", { 30,450 });
	UIMeter* HPBer4_ = GetUIMeter("HPBer4");
	HPBer4_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer4_->SetMeterMinMax(0.0f, 20.0f);
	HPBer4_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer4_->SetMeterType(UIMeterType::Down);

	InitUIMeter("HPBer5", { 30,550 });
	UIMeter* HPBer5_ = GetUIMeter("HPBer5");
	HPBer5_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer5_->SetMeterMinMax(0.0f, 20.0f);
	HPBer5_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer5_->SetMeterType(UIMeterType::WidthCenter);

	InitUIMeter("HPBer6", { 30,650 });
	UIMeter* HPBer6_ = GetUIMeter("HPBer6");
	HPBer6_->SetMaxSize({ 100,30 }, { 4.0f,4.0f });
	HPBer6_->SetMeterMinMax(0.0f, 20.0f);
	HPBer6_->GetMeterSprite()->SetColor({ 0,0,1,1 });
	HPBer6_->SetMeterType(UIMeterType::HeightCenter);

	InitUISlider("Slider",{300,200});

}

void PlayerUI::Update()
{
#ifdef _DEBUG
	ImGui::Begin("UI");
	ImGui::DragFloat("meter", &srideM_);
	ImGui::End();
#endif // _DEBUG

	UIMeter* hpber1 = GetUIMeter("HPBer1");
	UIMeter* hpber2 = GetUIMeter("HPBer2");
	UIMeter* hpber3 = GetUIMeter("HPBer3");
	UIMeter* hpber4 = GetUIMeter("HPBer4");
	UIMeter* hpber5 = GetUIMeter("HPBer5");
	UIMeter* hpber6 = GetUIMeter("HPBer6");
	hpber1->SetMeter(srideM_);
	hpber2->SetMeter(srideM_);
	hpber3->SetMeter(srideM_);
	hpber4->SetMeter(srideM_);
	hpber5->SetMeter(srideM_);
	hpber6->SetMeter(srideM_);



	GetUICheckBox("check1")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	GetUICheckBox("check2")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	GetUICheckBox("check3")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	GetUICheckBox("check4")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	GetUICheckBox("check5")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);



	GetUISlider("Slider")->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);

	UpdateUIElement(0);



	

}

void PlayerUI::Draw()
{
	/*UIMeter* hpber = GetUIMeter("HPBer");
	UIMeter* specailBer_ = GetUIMeter("SpecailBer");
	hpber->SetMeter(sizeHP_);
	specailBer_->SetMeter(sizeSpecialGauge_);*/

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
