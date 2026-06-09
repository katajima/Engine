#include "ResultUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>
#include "DirectXGame/application/base/Input/InputSystem.h"

void ResultUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables) {
	// 外部から受け取った管理クラスを保持し、更新処理で使えるようにする
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;

	// タイトルへ戻るボタンの背景スプライトを作成する
	titleUI_ = std::make_unique<Engine::Sprite>();
	titleUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	titleUI_->SetPosition(titlePos_);
	titleUI_->SetAnchorPoint(uiAnchorPoint_);
	titleUI_->SetTextureSize(uiSize_);
	titleUI_->SetSize(uiSize_);
	titleUI_->SetColor(uiColor_);
	// タイトルへ戻るボタンの文字スプライトを作成する
	titleUItxt_ = std::make_unique<Engine::Sprite>();
	titleUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectTitle.dds");
	titleUItxt_->SetPosition(titlePos_);
	titleUItxt_->SetAnchorPoint(uiAnchorPoint_);
	titleUItxt_->SetTextureSize(uiSize_);
	titleUItxt_->SetSize(uiSize_);
	titleUItxt_->SetColor(preparationColor_);



	// ゲームプレイへ戻るボタンの背景スプライトを作成する
	gamePlayUI_ = std::make_unique<Engine::Sprite>();
	gamePlayUI_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/Image.dds");
	gamePlayUI_->SetPosition(gamePlayPos_);
	gamePlayUI_->SetAnchorPoint(uiAnchorPoint_);
	gamePlayUI_->SetTextureSize(uiSize_);
	gamePlayUI_->SetSize(uiSize_);
	gamePlayUI_->SetColor(uiColor_);

	// ゲームプレイへ戻るボタンの文字スプライトを作成する
	gamePlayUItxt_ = std::make_unique<Engine::Sprite>();
	gamePlayUItxt_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/selectGamePlay.dds");
	gamePlayUItxt_->SetPosition(gamePlayPos_);
	gamePlayUItxt_->SetAnchorPoint(uiAnchorPoint_);
	gamePlayUItxt_->SetTextureSize(uiSize_);
	gamePlayUItxt_->SetSize(uiSize_);
	gamePlayUItxt_->SetColor(preparationColor_);


}

void ResultUI::Update(float dt) {
	// 未使用のdtを受け取り、他UIと同じ更新関数の形を保つ
	(void)dt;

	// まず通常色に戻してから、選択中の項目だけ強調色にする
	titleUI_->SetColor(uiColor_);
	gamePlayUI_->SetColor(uiColor_);
	if (selectedIndex == 0) {
		gamePlayUI_->SetColor(uiSelectColor_);
	}
	else if(selectedIndex == 1){
		titleUI_->SetColor(uiSelectColor_);
	}


	// 背景と文字のスプライトをそれぞれ更新する
	titleUI_->Update();
	titleUItxt_->Update();

	gamePlayUI_->Update();
	gamePlayUItxt_->Update();
}

void ResultUI::Draw() {
	// 背景を先に描き、その上に文字を重ねる
	titleUI_->Draw();
	titleUItxt_->Draw();

	gamePlayUI_->Draw();
	gamePlayUItxt_->Draw();
}
