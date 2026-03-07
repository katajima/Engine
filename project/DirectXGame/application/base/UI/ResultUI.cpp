#include "ResultUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

void ResultUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;

	titleUI_ = std::make_unique<Engine::Sprite>();
	titleUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	titleUI_->SetPosition(titlePos_);
	titleUI_->SetAnchorPoint(uiAnchorPoint_);
	titleUI_->SetTextureSize(uiSize_);
	titleUI_->SetSize(uiSize_);
	titleUI_->SetColor(uiColor_);
	titleUItxt_ = std::make_unique<Engine::Sprite>();
	titleUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectTitle.dds");
	titleUItxt_->SetPosition(titlePos_);
	titleUItxt_->SetAnchorPoint(uiAnchorPoint_);
	titleUItxt_->SetTextureSize(uiSize_);
	titleUItxt_->SetSize(uiSize_);
	titleUItxt_->SetColor(preparationColor_);



	gamePlayUI_ = std::make_unique<Engine::Sprite>();
	gamePlayUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	gamePlayUI_->SetPosition(gamePlayPos_);
	gamePlayUI_->SetAnchorPoint(uiAnchorPoint_);
	gamePlayUI_->SetTextureSize(uiSize_);
	gamePlayUI_->SetSize(uiSize_);
	gamePlayUI_->SetColor(uiColor_);

	gamePlayUItxt_ = std::make_unique<Engine::Sprite>();
	gamePlayUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectGamePlay.dds");
	gamePlayUItxt_->SetPosition(gamePlayPos_);
	gamePlayUItxt_->SetAnchorPoint(uiAnchorPoint_);
	gamePlayUItxt_->SetTextureSize(uiSize_);
	gamePlayUItxt_->SetSize(uiSize_);
	gamePlayUItxt_->SetColor(preparationColor_);


}

void ResultUI::Update(float dt) {
	titleUI_->SetColor(uiColor_);
	gamePlayUI_->SetColor(uiColor_);
	if (selectedIndex == 0) {
		gamePlayUI_->SetColor(uiSelectColor_);
	}
	else if(selectedIndex == 1){
		titleUI_->SetColor(uiSelectColor_);
	}


	titleUI_->Update();
	titleUItxt_->Update();

	gamePlayUI_->Update();
	gamePlayUItxt_->Update();
}

void ResultUI::Draw() {
	titleUI_->Draw();
	titleUItxt_->Draw();

	gamePlayUI_->Draw();
	gamePlayUItxt_->Draw();
}
