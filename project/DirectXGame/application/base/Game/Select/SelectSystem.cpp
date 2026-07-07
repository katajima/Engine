#include "SelectSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

void SelectSystem::Initialize(Engine::SceneManager* sceneManager ,InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	// 現在はメニュー操作で使用しない依存を明示的に未使用とする。
	(void)entityManager;
	(void)globalVariables;
	// 縦軸で4項目を選択し、各項目に対応する遷移先を設定する。
	MenuSelectionConfig config{};
	config.itemCount = 4;
	config.axis = MenuSelectionAxis::kVertical;
	config.positiveDirectionStep = -1;
	config.transitionScenes = { "GAMEPLAY", "", "", "TITLE" };
	menuSelectionController_.Initialize(sceneManager, input, config);
}

void SelectSystem::Update(float dt) {

	// 共通メニュー操作へ更新を委譲する。
	menuSelectionController_.Update(dt);
}
