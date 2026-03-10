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

	fade_->SetColor(uiColor_);
	fade_->Update();
}

void GameStartUI::Draw() {
	fade_->Draw();
}
