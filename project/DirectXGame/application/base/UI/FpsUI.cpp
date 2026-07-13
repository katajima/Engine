#include "FpsUI.h"

void FpsUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables){
	this->entityManager = entityManager;	// エンティティ
	this->globalVariables = globalVariables;	// 保存項目
	this->inputSystem = inputSystem;			// インプット



	// カウントUI
	InitUICount("fps", {}, 2);
	Engine::UICount* fpsCount = GetUICount("fps");
	fpsCount->Init(entityManager, "fps");	// 初期化
	fpsCount->SetInput(inputSystem);					// 入力設定
	fpsCount->SetPos({ 32,48 });				// 位置設定
	fpsCount->SetMaxSize({ 64 * 2 / 3, 96 * 2 / 3 }, { 20.0f,0.0f });	// 最大値サイズ設定
	fpsCount->SetTextureSize({ 64,96 });	// テクスチャサイズ設定
	fpsCount->SetCountMax(999);				// カウント量設定

}


// 更新
void FpsUI::Update(float dt) {

	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	nowTime += deltaTime;
	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}
	lastTime = currentTime;

	Engine::UICount* fpsCount = GetUICount("fps");
	// fpsカウント設定
	fpsCount->SetCount(fps);


	// 更新
	UpdateUIElement(0);
};

// 描画
void FpsUI::Draw() {
	// 描画
	DrawUIElement();
};