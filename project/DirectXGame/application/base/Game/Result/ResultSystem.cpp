#include "ResultSystem.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
#include "DirectXGame/engine/scene/SceneManager.h"
#include <algorithm>

namespace {
	// リザルト画面に表示するメニュー項目数。
	constexpr int kResultMenuItemCount = 2;
	// 右入力で次の項目へ移動するためのインデックス変化量。
	constexpr int kResultPositiveDirectionStep = 1;
	// シーン表示直後の誤決定を防止する待機時間。
	constexpr float kResultDecisionDelaySeconds = 0.5f;
	// スコアを加算表示しきるまでの時間。
	constexpr float kScoreCountDurationSeconds = 1.0f;
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
	menuSelectionController_.Initialize(sceneManager, input ? input->GetGameInputDataAddress() : nullptr, config);
	// リザルト開始時はスコア加算表示から始める。
	phase_ = ResultPhase::kScoreCount;
	phaseTimer_ = 0.0f;
	displayScore_ = 0.0f;
}

void ResultSystem::Update(float dt, float targetScore) {
	// 現在のフェーズだけを更新し、リザルト画面の進行を分かりやすく分離する。
	switch (phase_) {
	case ResultPhase::kScoreCount:
		UpdateScoreCount(dt, targetScore);
		break;
	case ResultPhase::kMenuWait:
		UpdateMenuWait(dt);
		break;
	}
}

void ResultSystem::ChangePhase(ResultPhase nextPhase) {
	// 新しいフェーズの経過時間を0から数えるためにタイマーをリセットする。
	phase_ = nextPhase;
	phaseTimer_ = 0.0f;
}

void ResultSystem::UpdateScoreCount(float dt, float targetScore) {
	// フェーズ内タイマーを進め、指定時間で最終スコアへ到達する割合を作る。
	phaseTimer_ += dt;
	const float countRate = std::clamp(phaseTimer_ / kScoreCountDurationSeconds, 0.0f, 1.0f);
	displayScore_ = targetScore * countRate;

	// スコア表示が完了したら最終値に固定し、メニュー入力待ちへ進める。
	if (countRate >= 1.0f) {
		displayScore_ = targetScore;
		ChangePhase(ResultPhase::kMenuWait);
	}
}

void ResultSystem::UpdateMenuWait(float dt) {
	// 共通メニュー操作へ更新を委譲する。
	menuSelectionController_.Update(dt);
}
