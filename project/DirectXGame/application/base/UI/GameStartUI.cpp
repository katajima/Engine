#include "GameStartUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

namespace {
	// フェードUIの1秒当たりの透明度変化量。
	constexpr float kFadeAlphaSpeed = 2.0f;
	// UI透明度の有効範囲。
	constexpr float kMinAlpha = 0.0f;
	constexpr float kMaxAlpha = 1.0f;
	// カウント数字テクスチャの1文字分のサイズ。
	const Vector2 kCountTextureSize = { 64.0f, 96.0f };
	// カウントUIが表示できる最大数字。
	constexpr int kMaximumCountDigit = 9;
}

void GameStartUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;

	// UI
	fade_ = std::make_unique<Engine::Sprite>();
	fade_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	fade_->SetPosition(position_);
	fade_->SetAnchorPoint(uiAnchorPoint_);
	fade_->SetTextureSize(uiSize_);
	fade_->SetSize(uiSize_);
	fade_->SetColor(uiColor_);

	// ゲーム準備前UI
	gameStartPreparationUI_ = std::make_unique<Engine::Sprite>();
	gameStartPreparationUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/gameStartPreparation.dds");
	gameStartPreparationUI_->SetPosition(gameStartPosition_);
	gameStartPreparationUI_->SetAnchorPoint(uiAnchorPoint_);
	gameStartPreparationUI_->SetTextureSize(gameStartUiSize_);
	gameStartPreparationUI_->SetSize(gameStartUiSize_);
	gameStartPreparationUI_->SetColor(numUiColor_);
	
	// カウントUI
	InitUICount("numCount", numPosition_, 1, false);
	Engine::UICount* hitCount = GetUICount("numCount");
	hitCount->SetMaxSize(numUiSize_, {});	// 最大サイズ
	hitCount->SetTextureSize(kCountTextureSize);		// テクスチャサイズ
	hitCount->SetCountMax(kMaximumCountDigit);		// カウント最大数
	hitCount->SetCountColor(numUiColor_);	// 色指定
}

void GameStartUI::Update(float dt) {
	if (isfade_) {
		uiColor_.a += kFadeAlphaSpeed * dt;
		if (uiColor_.a >= kMaxAlpha) {
			uiColor_.a = kMaxAlpha;
		}
	}
	else {
		uiColor_.a -= kFadeAlphaSpeed * dt;
		if (uiColor_.a <= kMinAlpha) {
			uiColor_.a = kMinAlpha;
		}
	}

	Engine::UICount* hitCount = GetUICount("numCount");
	hitCount->SetMaxSize(numUiSize_, {});
	hitCount->SetCountColor(numUiColor_);
	hitCount->SetCount(num);
	hitCount->Update(dt);

	fade_->SetColor(uiColor_);
	fade_->Update();

	gameStartPreparationUI_->Update();
}

void GameStartUI::Draw() {
	Engine::UICount* hitCount = GetUICount("numCount");
	if (isNum) {
	//	hitCount->Draw();
	}

	gameStartPreparationUI_->Draw();

	fade_->Draw();
}
