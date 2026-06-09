#pragma once

#include "DirectXGame/application/base/Effect/SlowMotion/SlowMotionRequest.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

namespace Character {

	// スロー演出リクエストを受け取り、ゲーム全体の時間倍率を管理するクラス
	class SlowMotionManager {
	public:
		// 破棄時に時間倍率が残らないよう、管理中のスローを解除する
		~SlowMotionManager() { Stop(); }

		// スロー演出リクエストを受け付ける
		void Request(const SlowMotionRequest& request) {
			if (!isActive_) {
				restoreTimeScale_ = Engine::MyGame::GetTimeScale();	// 演出前の時間倍率を退避する
			}
			else if (request.GetPriority() < activePriority_) {
				return;	// 優先度が低いリクエストは現在のスローを邪魔しない
			}

			isActive_ = true;
			timer_ = 0.0f;
			duration_ = request.GetDuration();
			activePriority_ = request.GetPriority();
			Engine::MyGame::SetTimeScale(request.GetTimeScale());
		}

		// スロー演出の時間を進め、終了したら元の時間倍率へ戻す
		void Update() {
			if (!isActive_) {
				return;
			}

			// スロー中でも演出の寿命は実時間基準で進める
			timer_ += Engine::MyGame::BaseDeltaTime();
			if (timer_ >= duration_) {
				Stop();
			}
		}

		// 現在のスロー演出を即座に解除する
		void Stop() {
			if (!isActive_) {
				return;
			}

			Engine::MyGame::SetTimeScale(restoreTimeScale_);
			isActive_ = false;
			timer_ = 0.0f;
			duration_ = 0.0f;
			activePriority_ = 0;
		}

	private:
		bool isActive_ = false;			// スロー演出を適用中か
		float timer_ = 0.0f;			// 現在のスロー演出の経過時間
		float duration_ = 0.0f;			// 現在のスロー演出の継続時間
		float restoreTimeScale_ = 1.0f;	// スロー終了時に戻す時間倍率
		int activePriority_ = 0;		// 現在適用中のスロー優先度
	};
}
