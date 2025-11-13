#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void PlayerUI::Initialize(Input* input, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目
	input_ = input;						// インプット


	InitUIMeter("HPBer",{30,600}, true);		// 初期化HP
	InitUIMeter("SpecailBer",{30,650}, true);	// 初期化スペシャル
	InitUIMeter("StaminaBer",{30,700}, true);	// 初期化スタミナ


	// HPUI
	UIMeter* hpber  = GetUIMeter("HPBer");
	hpber->SetMaxSize({ 200,30 },{4.0f,4.0f });									// 最大サイズ
	hpber->SetMeterMinMax(0.0f,100.0f);											// メータ最大値最小値
	hpber->GetMeterSprite()->SetColor({ 0,1,0,1 });								// 色指定
	hpber->SetMeterType(UIMeterType::Left);										// メータの増える方向
	hpber->GetNameSprite()->SetTextureName("resources/Texture/text/HP.png");	// 次のスプライト設定
	hpber->GetNameSprite()->SetSize({60,40});									// サイズ設定
	hpber->GetNameSprite()->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });				// 色指定

	// スペシャルUI
	UIMeter* specailBer_ = GetUIMeter("SpecailBer");
	specailBer_->SetMaxSize({ 200,30 },{4.0f,4.0f });								// 最大サイズ
	specailBer_->SetMeterMinMax(0.0f, 100.0f);										// メータ最大値最小値
	specailBer_->GetMeterSprite()->SetColor({ 0,0,1,1 });							// 色指定
	specailBer_->SetMeterType(UIMeterType::Left);									// メータの増える方向
	specailBer_->GetNameSprite()->SetTextureName("resources/Texture/text/SP.png");	// 次のスプライト設定
	specailBer_->GetNameSprite()->SetSize({ 60,40 });								// サイズ設定
	specailBer_->GetNameSprite()->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });			// 色指定

	// スタミナUI
	UIMeter* staminaBer_ = GetUIMeter("StaminaBer");
	staminaBer_->SetMaxSize({ 200,30 },{4.0f,4.0f });								// 最大サイズ
	staminaBer_->SetMeterMinMax(0.0f, 20.0f);										// メータ最大値最小値
	staminaBer_->GetMeterSprite()->SetColor({ 1,1,0,1 });							// 色指定
	staminaBer_->SetMeterType(UIMeterType::Left);									// メータの増える方向
	staminaBer_->GetNameSprite()->SetTextureName("resources/Texture/text/SP.png");	// 次のスプライト設定
	staminaBer_->GetNameSprite()->SetSize({ 60,40 });								// サイズ設定
	staminaBer_->GetNameSprite()->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });			// 色指定


	// maxテキストスプライト初期化
	textMax_ = std::make_unique<Sprite>();
	textMax_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/text/max.png");
	textMax_->SetColor({ 1,0,0,1 });					// 色設定
	textMax_->SetPosition({ 30,700 });					// 位置設定
	textMax_->SetRotation(Math::DegreesToRadians(-30));	// 回転設定
	textMax_->SetAnchorPoint({ 0.5f,0.5f });			// アンカーポイント設定
	textMax_->SetSize(0.25f);							// サイズ設定

	// rbテキストスプライト初期化
	textRB_ = std::make_unique<Sprite>();
	textRB_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/icon/RB.png");
	textRB_->SetColor({ 1,1,1,1 });						// 色指定
	textRB_->SetPosition({ 1280 / 2,550 });				// 位置設定
	textRB_->SetAnchorPoint({ 0.5f,0.5f });				// アンカーポイント設定
	textRB_->SetSize(0.2f);								// サイズ設定

}

void PlayerUI::Update()
{
	UIMeter* hpber = GetUIMeter("HPBer");			// HP
	UIMeter* specailBer = GetUIMeter("SpecailBer");	// スペシャル
	UIMeter* staminaBer = GetUIMeter("StaminaBer");	// スタミナ
	hpber->SetMeterMinMax(hpBar_->minValue, hpBar_->maxValue);	// メータ最大値
	hpber->SetMeter(hpBar_->value);								// メータ
	
	staminaBer->SetMeterMinMax(staminaBar_->minValue, staminaBar_->maxValue);	// メータ最大値
	staminaBer->SetMeter(staminaBar_->value);									// メータ


	specailBer->SetMeter(sizeSpecialGauge_);// メータ

	// 更新
	UpdateUIElement(0);
}

void PlayerUI::Draw()
{
	// 描画
	DrawUIElement();
	

	// テキストMAX
	if (isTextMax_) {
		textMax_->Update();
		textMax_->Draw();
	};

	// テキストRB
	if (isTextRB_) {
		textRB_->Update();
		textRB_->Draw();
	}
}
