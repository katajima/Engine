#include "ResultScoreUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

void ResultScoreUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables) {
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;

	// スコア
	InitUICount("scoreUI", uiPos,4,true);
	Engine::UICount* scoreUI = GetUICount("scoreUI");
	scoreUI->SetMaxSize(uiSize_, offset);	// 最大サイズ
	
	scoreUI->SetTextuerSize(uiSize_);		// テクスチャサイズ
	scoreUI->SetCountMax(99999);							// カウント最大数
	scoreUI->SetCountColor(uiColor_);	// 色指定
	scoreUI->GetNameSprite()->SetTextureName("resources/Texture/text/textScore.dds");
	scoreUI->GetNameSprite()->SetSize(uiNameSize);	// サイズ指定
	scoreUI->GetNameSprite()->SetColor(uiColor_);	// 色指定

}

void ResultScoreUI::Update(float dt) {
	Engine::UICount* scoreUI = GetUICount("scoreUI");
	scoreUI->SetCount(score);

	// 更新
	UpdateUIElement(dt);
}

void ResultScoreUI::Draw() {
	// 描画
	DrawUIElement();
}
