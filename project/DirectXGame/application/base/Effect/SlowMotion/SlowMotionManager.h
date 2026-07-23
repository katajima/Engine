#pragma once

#include "DirectXGame/application/base/Effect/SlowMotion/SlowMotionRequest.h"
#include <vector>

namespace Character {
	class BaseCharacter;
}

namespace Character {

	// スロー演出リクエストを受け取り、対象キャラクターの時間倍率を管理するクラス
/// <summary>
/// SlowMotionManagerを管理・実装するクラス。
/// </summary>
	class SlowMotionManager {
	public:
		// 破棄時に内部状態を停止する
		~SlowMotionManager();

		// スロー演出リクエストを受け付ける
		void Request(const SlowMotionRequest& request);

		// スロー演出の時間を進め、対象キャラクターへ時間倍率を反映する
		void Update(const std::vector<const BaseCharacter*>& targets);

		// 現在のスロー演出を即座に解除する
		void Stop();

	private:
		// 対象キャラクターへ現在の時間倍率を反映する
		void ApplyTargets(const std::vector<const BaseCharacter*>& targets, float timeScale);

	private:
		bool isActive_ = false;			// スロー演出を適用中か
		float timer_ = 0.0f;			// 現在のスロー演出の経過時間
		float duration_ = 0.0f;			// 現在のスロー演出の継続時間
		float targetTimeScale_ = 1.0f;	// 対象キャラクターへ適用する時間倍率
		float normalTimeScale_ = 1.0f;	// スロー終了時に対象キャラクターへ戻す時間倍率
		int activePriority_ = 0;		// 現在適用中のスロー優先度
	};
}
