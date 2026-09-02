#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "DirectXGame/engine/input/InputData.h"
#include "DirectXGame/engine/scene/SceneManager.h"

/// <summary>
/// メニュー項目の移動に使用するスティック軸。
/// </summary>
enum class MenuSelectionAxis {
	kHorizontal,
	kVertical,
};

/// <summary>
/// メニュー選択操作を構成する設定値。
/// </summary>
struct MenuSelectionConfig {
	// 選択可能な項目数。
	int itemCount = 1;
	// 項目移動に使用するスティック軸。
	MenuSelectionAxis axis = MenuSelectionAxis::kVertical;
	// 軸の正方向へ入力した際に加算するインデックス値。
	int positiveDirectionStep = 1;
	// 入力として扱うスティック値のしきい値。
	float inputThreshold = 0.5f;
	// 項目移動を繰り返せる間隔。
	float moveInterval = 0.25f;
	// 初期化直後に決定入力を受け付けない時間。
	float decisionDelay = 0.0f;
	// 項目インデックスに対応する遷移先。空文字の項目は遷移しない。
	std::vector<std::string> transitionScenes;
};

/// <summary>
/// 設定に従ってメニュー選択とシーン遷移を処理する共通クラス。
/// </summary>
class MenuSelectionController {
public:
	/// <summary>
	/// 入力元、遷移先管理、メニュー構成を設定する。
	/// </summary>
	void Initialize(Engine::SceneManager* sceneManager, const Engine::MenuInputData* input, const MenuSelectionConfig& config) {
		// 更新時に利用する外部システムを保持する。
		sceneManager_ = sceneManager;
		input_ = input;
		config_ = config;
		/// <summary>
		/// 不正な項目数でも選択範囲が成立するよう最低値を保証する。
		/// </summary>
		config_.itemCount = (std::max)(config_.itemCount, 1);
		selectedIndex_ = 0;
		moveTimer_ = 0.0f;
		decisionTimer_ = 0.0f;
	}

	/// <summary>
	/// 軸入力による項目移動と、決定入力によるシーン遷移を更新する。
	/// </summary>
	void Update(float dt) {
		// 移動入力と決定入力の経過時間を進める。
		moveTimer_ += dt;
		decisionTimer_ += dt;

		/// <summary>
		/// 設定された軸の値だけを選択操作へ利用する。
		/// </summary>
		const Vector2 moveStick = input_->moveShick;
		const float axisValue = config_.axis == MenuSelectionAxis::kHorizontal ? moveStick.x : moveStick.y;
		if (moveTimer_ >= config_.moveInterval) {
			if (axisValue > config_.inputThreshold) {
				selectedIndex_ += config_.positiveDirectionStep;
				moveTimer_ = 0.0f;
			} else if (axisValue < -config_.inputThreshold) {
				selectedIndex_ -= config_.positiveDirectionStep;
				moveTimer_ = 0.0f;
			}
		}
		/// <summary>
		/// 選択位置が項目範囲を超えないよう補正する。
		/// </summary>
		selectedIndex_ = std::clamp(selectedIndex_, 0, config_.itemCount - 1);

		// 誤決定防止時間の経過後だけ、設定された遷移先へ移動する。
		if (decisionTimer_ >= config_.decisionDelay && input_->decisionTrigger &&
			selectedIndex_ < static_cast<int>(config_.transitionScenes.size())) {
			const std::string& transitionScene = config_.transitionScenes[selectedIndex_];
			if (!transitionScene.empty()) {
				sceneManager_->ChangeScene(transitionScene);
			}
		}
	}

	/// <summary>
	/// 現在選択されている項目インデックスを取得する。
	/// </summary>
	int GetSelectedIndex() const { return selectedIndex_; }

private:
	// シーン遷移を実行する管理クラス。
	Engine::SceneManager* sceneManager_ = nullptr;
	// メニュー操作に使用するエンジン共通入力。
	const Engine::MenuInputData* input_ = nullptr;
	// 項目数、軸、遷移先をまとめた設定。
	MenuSelectionConfig config_;
	// 現在選択されている項目インデックス。
	int selectedIndex_ = 0;
	// 項目移動の連続入力を制限するタイマー。
	float moveTimer_ = 0.0f;
	// 初期決定入力を抑制するタイマー。
	float decisionTimer_ = 0.0f;
};
