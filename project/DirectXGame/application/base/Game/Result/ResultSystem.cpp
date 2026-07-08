#include "ResultSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"

namespace {
	// リザルト画面に表示するメニュー項目数。
	constexpr int kResultMenuItemCount = 2;
	// 右入力で次の項目へ移動するためのインデックス変化量。
	constexpr int kResultPositiveDirectionStep = 1;
	// シーン表示直後の誤決定を防止する待機時間。
	constexpr float kResultDecisionDelaySeconds = 0.5f;
}

void ResultSystem::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables) {
	// 現在はメニュー操作で使用しない依存を明示的に未使用とする。
	(void)entityManager;
	(void)globalVariables;
	// 横軸で2項目を選択し、誤決定防止時間と遷移先を設定する。
	MenuSelectionConfig config{};
	config.itemCount = kResultMenuItemCount;
	config.axis = MenuSelectionAxis::kHorizontal;
	config.positiveDirectionStep = kResultPositiveDirectionStep;
	config.decisionDelay = kResultDecisionDelaySeconds;
	config.transitionScenes = { "GAMEPLAY", "TITLE" };
	menuSelectionController_.Initialize(sceneManager, input, config);
}

void ResultSystem::Update(float dt) {

	// 共通メニュー操作へ更新を委譲する。
	menuSelectionController_.Update(dt);
}
