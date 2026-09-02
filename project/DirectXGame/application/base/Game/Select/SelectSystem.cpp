#include "SelectSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

namespace {
	// セレクト画面に表示するメニュー項目数。
	constexpr int kSelectMenuItemCount = 4;
	// 上入力で前の項目へ移動するためのインデックス変化量。
	constexpr int kSelectPositiveDirectionStep = -1;
}

void SelectSystem::Initialize(Engine::SceneManager* sceneManager ,InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	// 現在はメニュー操作で使用しない依存を明示的に未使用とする。
	(void)entityManager;
	(void)globalVariables;
	// 縦軸で4項目を選択し、各項目に対応する遷移先を設定する。
	MenuSelectionConfig config{};
	config.itemCount = kSelectMenuItemCount;
	config.axis = MenuSelectionAxis::kVertical;
	config.positiveDirectionStep = kSelectPositiveDirectionStep;
	config.transitionScenes = { "GAMEPLAY", "", "", "TITLE" };
	menuSelectionController_.Initialize(sceneManager, input ? input->GetGameInputDataAddress() : nullptr, config);
}

void SelectSystem::Update(float dt) {

	// 共通メニュー操作へ更新を委譲する。
	menuSelectionController_.Update(dt);
}
