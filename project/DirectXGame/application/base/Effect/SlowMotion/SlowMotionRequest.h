#pragma once

namespace Character {

	// 対象スロー演出管理クラスへ渡す、1回分のスロー演出リクエスト
	class SlowMotionRequest {
	public:
		// 回避成功用の対象スロー演出リクエストを生成する
		static SlowMotionRequest CreateDodgeSuccess() {
			SlowMotionRequest request;	// 回避成功時に使うスロー設定
			request.timeScale_ = 0.10f;
			request.duration_ = 1.0f;
			request.priority_ = 10;
			return request;
		}

		// 範囲爆撃必殺技中に敵の時間を停止するリクエストを生成する
		static SlowMotionRequest CreateRangeBombingTimeStop() {
			SlowMotionRequest request;		// 必殺技のphase0〜1が続く間、毎フレーム更新して停止を維持する
			request.timeScale_ = 0.0f;
			request.duration_ = 0.1f;
			request.priority_ = 20;
			return request;
		}

		// 対象へ適用したい時間倍率を取得する
		float GetTimeScale() const { return timeScale_; }

		// スロー演出を維持する時間を取得する
		float GetDuration() const { return duration_; }

		// 同時に複数リクエストが来た時の優先度を取得する
		int GetPriority() const { return priority_; }

	private:
		float timeScale_ = 1.0f;	// リクエスト中に対象へ適用する時間倍率
		float duration_ = 0.0f;	// リクエストの継続時間
		int priority_ = 0;		// 現在のスローより優先して上書きできるかの値
	};
}
