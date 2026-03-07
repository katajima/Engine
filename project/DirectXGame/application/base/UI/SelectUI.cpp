#include "SelectUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

void SelectUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;


	mainGameUI_ = std::make_unique<Engine::Sprite>();
	mainGameUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	mainGameUI_->SetPosition(mainGameUIPos_);
	mainGameUI_->SetAnchorPoint(uiAnchorPoint_);
	mainGameUI_->SetTextureSize(uiSize_);
	mainGameUI_->SetSize(uiSize_);
	mainGameUI_->SetColor(uiColor_);

	mainGameUItxt_ = std::make_unique<Engine::Sprite>();
	mainGameUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectGamePlay.dds");
	mainGameUItxt_->SetPosition(mainGameUIPos_);
	mainGameUItxt_->SetAnchorPoint(uiAnchorPoint_);
	mainGameUItxt_->SetTextureSize(uiSize_);
	mainGameUItxt_->SetSize(uiSize_);
	mainGameUItxt_->SetColor(uiColor_);

	customUI_ = std::make_unique<Engine::Sprite>();
	customUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	customUI_->SetPosition(customGameUIPos_);
	customUI_->SetAnchorPoint(uiAnchorPoint_);
	customUI_->SetTextureSize(uiSize_);
	customUI_->SetSize(uiSize_);
	customUI_->SetColor(uiColor_);

	titleUI_ = std::make_unique<Engine::Sprite>();
	titleUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	titleUI_->SetPosition(titleUIPos_);
	titleUI_->SetAnchorPoint(uiAnchorPoint_);
	titleUI_->SetTextureSize(uiSize_);
	titleUI_->SetSize(uiSize_);
	titleUI_->SetColor(uiColor_);

	titleUItxt_ = std::make_unique<Engine::Sprite>();
	titleUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectTitle.dds");
	titleUItxt_->SetPosition(titleUIPos_);
	titleUItxt_->SetAnchorPoint(uiAnchorPoint_);
	titleUItxt_->SetTextureSize(uiSize_);
	titleUItxt_->SetSize(uiSize_);
	titleUItxt_->SetColor(uiColor_);

	operationUI_ = std::make_unique<Engine::Sprite>();
	operationUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	operationUI_->SetPosition(operationUIPos_);
	operationUI_->SetAnchorPoint(uiAnchorPoint_);
	operationUI_->SetTextureSize(uiSize_);
	operationUI_->SetSize(uiSize_);
	operationUI_->SetColor(uiColor_);

	preparationUI01_ = std::make_unique<Engine::Sprite>();
	preparationUI01_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/preparation.dds");
	preparationUI01_->SetPosition(customGameUIPos_);
	preparationUI01_->SetAnchorPoint(uiAnchorPoint_);
	preparationUI01_->SetTextureSize(uiSize_);
	preparationUI01_->SetSize(uiSize_);
	preparationUI01_->SetColor(preparationColor_);



	preparationUI03_ = std::make_unique<Engine::Sprite>();
	preparationUI03_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/preparation.dds");
	preparationUI03_->SetPosition(operationUIPos_);
	preparationUI03_->SetAnchorPoint(uiAnchorPoint_);
	preparationUI03_->SetTextureSize(uiSize_);
	preparationUI03_->SetSize(uiSize_);
	preparationUI03_->SetColor(preparationColor_);

};

//更新
void SelectUI::Update(float dt) {
	mainGameUI_->SetColor(uiColor_);
	mainGameUItxt_->SetColor(preparationColor_);
	customUI_->SetColor(uiColor_);
	titleUI_->SetColor(uiColor_);
	titleUItxt_->SetColor(preparationColor_);
	operationUI_->SetColor(uiColor_);

	

	if (selectedIndex == 0) {
		mainGameUI_->SetColor(uiSelectColor_);
	}
	if (selectedIndex == 1) {
		customUI_->SetColor(uiSelectColor_);
	}
	if (selectedIndex == 2) {
		operationUI_->SetColor(uiSelectColor_);
	}
	if (selectedIndex == 3) {
		titleUI_->SetColor(uiSelectColor_);
	}


	preparationUI01_->Update();
	preparationUI03_->Update();


	mainGameUI_->Update();
	mainGameUItxt_->Update();
	customUI_->Update();
	titleUI_->Update();
	titleUItxt_->Update();

	operationUI_->Update();
};

// 描画
void SelectUI::Draw() {
	mainGameUI_->Draw();
	mainGameUItxt_->Draw();

	customUI_->Draw();

	titleUI_->Draw();
	titleUItxt_->Draw();

	operationUI_->Draw();

	preparationUI01_->Draw();
	preparationUI03_->Draw();

};