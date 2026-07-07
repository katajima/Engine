#include "ResultSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void ResultSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	// 現在はメニュー操作で使用しない依存を明示的に未使用とする。
	(void)entityManager;
	(void)globalVariables;
	// 横軸で2項目を選択し、誤決定防止時間と遷移先を設定する。
	MenuSelectionConfig config{};
	config.itemCount = 2;
	config.axis = MenuSelectionAxis::kHorizontal;
	config.positiveDirectionStep = 1;
	config.decisionDelay = 0.5f;
	config.transitionScenes = { "GAMEPLAY", "TITLE" };
	menuSelectionController_.Initialize(sceneManager, input, config);
}

void ResultSystem::Update(float dt) {

	// 共通メニュー操作へ更新を委譲する。
	menuSelectionController_.Update(dt);
}
