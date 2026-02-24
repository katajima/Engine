#include "TitleUI.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

void TitleUI::Initialize(InputSystem* inputSystem, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables){
	iconB_ = std::make_unique<Engine::Sprite>();
	iconB_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/icon/B.png");
	iconB_->SetPosition({ 640,500 });
	iconB_->SetAnchorPoint({ 0.5f,0.5f });
	iconB_->SetSize({ 200,200 });

	title_ = std::make_unique<Engine::Sprite>();
	title_->Initialize(entity2DManager->GetSpriteCommon(), "resources/Texture/text/title.png");
	title_->SetPosition({ 640,200 });
	title_->SetAnchorPoint({ 0.5f,0.5f });
	title_->SetSize(2);
}

void TitleUI::Update(float dt){
	// ボタンUI処理
	ButtonUiProcess(dt);

	// タイトルUI更新
	title_->Update();
}

void TitleUI::Draw(){
	iconB_->Draw();
	title_->Draw();
}

void TitleUI::ButtonUiProcess(float dt) {

	//buttonTimer_ += dt;

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
