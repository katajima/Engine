#include "TitleUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

void TitleUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) {
	iconB_ = std::make_unique<Engine::Sprite>();
	iconB_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/icon/B.dds");
	iconB_->SetPosition({ 640,600 });
	iconB_->SetAnchorPoint({ 0.5f,0.5f });
	iconB_->SetSize({ 200,200 });

	title_ = std::make_unique<Engine::Sprite>();
	title_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/title.dds");
	title_->SetPosition(titlePos_);
	title_->SetAnchorPoint({ 0.5f,0.5f });
	title_->SetSize(2);
	title_->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });
}

void TitleUI::Update(float dt) {
	// ボタンUI処理
	ButtonUiProcess(dt);
	// タイトルUI処理
	TitleUiProcess(dt);
}

void TitleUI::Draw() {
	iconB_->Draw();
	title_->Draw();
}

void TitleUI::ButtonUiProcess(float dt) {

	if (isAction_) {
		titlePos_.y -= 300.0f * dt;

		title_->SetPosition(titlePos_);

		if (buttonColor_.a <= 0.0f) {
			return;
		}
	}

	if (buttonColor_.a >= 1.0f) {
		buttonColor_.a = 1.0f;
		buttonClock_ *= -1;
	}
	else if (buttonColor_.a <= 0.0f) {
		buttonColor_.a = 0.0f;
		buttonClock_ *= -1;
	}

	if (buttonClock_ == 1) {
		buttonColor_.a += 0.015f;
	}
	else {
		buttonColor_.a -= 0.015f;
	}

	// 色設定
	iconB_->SetColor(buttonColor_);
	// UI更新
	iconB_->Update();
}

void TitleUI::TitleUiProcess(float dt) {
	// タイトルUI更新
	title_->Update();
}
