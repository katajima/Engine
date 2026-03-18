#include "GameStartUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

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

	// カウントUI
	InitUICount("numCount", numPosition_, 1, false);
	Engine::UICount* hitCount = GetUICount("numCount");
	hitCount->SetMaxSize(numUiSize_, {});	// 最大サイズ
	hitCount->SetTextuerSize({64,96});				// テクスチャサイズ
	hitCount->SetCountMax(9);							// カウント最大数
	hitCount->SetCountColor(numUiColor_);	// 色指定
}

void GameStartUI::Update(float dt) {
	if (isfade_) {
		uiColor_.a += 2.0f * dt;
		if (uiColor_.a >= 1.0f) {
			uiColor_.a = 1.0f;
		}
	}
	else {
		uiColor_.a -= 2.0f * dt;
		if (uiColor_.a <= 0.0f) {
			uiColor_.a = 0.0f;
		}
	}

	Engine::UICount* hitCount = GetUICount("numCount");
	hitCount->SetMaxSize(numUiSize_, {});
	hitCount->SetCountColor(numUiColor_);
	hitCount->SetCount(num);
	hitCount->Update(dt);

	fade_->SetColor(uiColor_);
	fade_->Update();

	
}

void GameStartUI::Draw() {
	Engine::UICount* hitCount = GetUICount("numCount");
	if (isNum) {
	//	hitCount->Draw();
	}
	fade_->Draw();
}
