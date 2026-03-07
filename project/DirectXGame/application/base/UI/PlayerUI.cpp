#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>

void PlayerUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->entityManager = entityManager;	// エンティティ
	this->globalVariables = globalVariables;	// 保存項目
	this->inputSystem = inputSystem;						// インプット


	InitUIMeter("HPBer", hpSpriteData_.pos_, true);		// 初期化HP
	InitUIMeter("SpecailBer",spSpriteData_.pos_, true);	// 初期化スペシャル
	

	// HPUI
	Engine::UIMeter* hpber  = GetUIMeter("HPBer");
	hpber->SetMaxSize(hpSpriteData_.size_, hpSpriteData_.offset_);									// 最大サイズ
	hpber->SetMeterMinMax(0.0f, hpSpriteData_.maxMeter);											// メータ最大値最小値
	hpber->GetMeterSprite()->SetColor(hpSpriteData_.color_);								// 色指定
	hpber->SetMeterType(UIMeterType::Left);										// メータの増える方向
	hpber->GetNameSprite()->SetTextureName("resources/Texture/text/HP.dds");	// 次のスプライト設定
	hpber->GetNameSprite()->SetSize(hpSpriteData_.nameSize_);									// サイズ設定
	hpber->GetNameSprite()->SetColor(hpSpriteData_.nameColor_);				// 色指定

	// スペシャルUI
	Engine::UIMeter* specailBer_ = GetUIMeter("SpecailBer");
	specailBer_->SetMaxSize(spSpriteData_.size_, spSpriteData_.offset_);								// 最大サイズ
	specailBer_->SetMeterMinMax(0.0f, spSpriteData_.maxMeter);										// メータ最大値最小値
	specailBer_->GetMeterSprite()->SetColor(spSpriteData_.color_);							// 色指定
	specailBer_->SetMeterType(UIMeterType::Left);									// メータの増える方向
	specailBer_->GetNameSprite()->SetTextureName("resources/Texture/text/SP.dds");	// 次のスプライト設定
	specailBer_->GetNameSprite()->SetSize(spSpriteData_.nameSize_);								// サイズ設定
	specailBer_->GetNameSprite()->SetColor(spSpriteData_.nameColor_);			// 色指定

	// maxテキストスプライト初期化
	textMax_ = std::make_unique<Engine::Sprite>();
	textMax_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/max.dds");
	textMax_->SetColor(maxTextData_.color_);					// 色設定
	textMax_->SetPosition(maxTextData_.pos_);					// 位置設定
	textMax_->SetRotation(maxTextData_.rotate_);	// 回転設定
	textMax_->SetAnchorPoint(maxTextData_.anchorPoint_);			// アンカーポイント設定
	textMax_->SetSize(maxTextData_.size_);							// サイズ設定

	// rbテキストスプライト初期化
	textRB_ = std::make_unique<Engine::Sprite>();
	textRB_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/icon/RB.dds");
	textRB_->SetColor(Color::WHITE());						// 色指定
	textRB_->SetPosition(rbData_.pos_);				// 位置設定
	textRB_->SetAnchorPoint(rbData_.anchorPoint_);				// アンカーポイント設定
	textRB_->SetSize(rbData_.size_);								// サイズ設定

}

void PlayerUI::Update(float dt)
{
	Engine::UIMeter* hpber = GetUIMeter("HPBer");			// HP
	Engine::UIMeter* specailBer = GetUIMeter("SpecailBer");	// スペシャル
	hpber->SetMeterMinMax(parameterComponent->HP().minValue, parameterComponent->HP().maxValue);	// メータ最大値
	hpber->SetMeter(parameterComponent->HP().value);								// メータ
	
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
