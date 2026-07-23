#pragma once
// C++
#include <imgui.h>
#include <list>
// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/application/base/Light/BaseLights.h"
#include "DirectXGame/application/base/Game/Menu/MenuSelectionController.h"

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class SceneManager;
}
class EffectSystem;
class InputSystem;

/// <summary>
/// リザルト画面の進行フェーズ。
/// </summary>
enum class ResultPhase {
	// スコアを0から最終値まで加算表示する。
	kScoreCount,
	// メニュー項目の選択と決定を受け付ける。
	kMenuWait,
};

/// <summary>
/// ResultSystemを管理・実装するクラス。
/// </summary>
class ResultSystem {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	void Initialize(Engine::SceneManager* sceneManager, InputSystem* input, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	/// <param name="targetScore"></param>
	void Update(float dt, float targetScore);

	/// <summary>
	/// 選択されているインデックスの取得
	/// </summary>
	/// <returns></returns>
	int GetSelectedIndex() const { return menuSelectionController_.GetSelectedIndex(); }

	/// <summary>
	/// UIに表示するスコアの取得
	/// </summary>
	/// <returns></returns>
	float GetDisplayScore() const { return displayScore_; }

	/// <summary>
	/// メニュー操作を表示できるか取得
	/// </summary>
	/// <returns></returns>
	bool IsMenuActive() const { return phase_ == ResultPhase::kMenuWait; }

private:
	/// <summary>
	/// フェーズを切り替える
	/// </summary>
	/// <param name="nextPhase"></param>
	void ChangePhase(ResultPhase nextPhase);

	/// <summary>
	/// スコア加算表示フェーズを更新する
	/// </summary>
	/// <param name="dt"></param>
	/// <param name="targetScore"></param>
	void UpdateScoreCount(float dt, float targetScore);

	/// <summary>
	/// メニュー入力待ちフェーズを更新する
	/// </summary>
	/// <param name="dt"></param>
	void UpdateMenuWait(float dt);

private:
	// リザルト画面の項目移動とシーン遷移を処理する。
	MenuSelectionController menuSelectionController_;
	// 現在のリザルト進行フェーズ。
	ResultPhase phase_ = ResultPhase::kScoreCount;
	// フェーズ内で経過した時間。
	float phaseTimer_ = 0.0f;
	// UIに表示するための加算中スコア。
	float displayScore_ = 0.0f;
};
