#pragma once
#include <DirectXGame/engine/struct/Structs3D.h>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// ウェーブ終了時の敵退場ステート。
	/// 撃破演出とは分離し、静かに縮小・沈下して次のウェーブへ場を譲る。
	/// </summary>
	class EnemyWaveExitState {
	public:
		// 退場開始時の姿勢を保存する
		void Enter(BaseEnemy* enemy, float duration = 1.2f);
		// 退場演出を進め、完了時に敵を削除可能な状態へする
		void Update(BaseEnemy* enemy, float dt);

		bool IsActive() const { return isActive_; }
		bool HasStarted() const { return hasStarted_; }

	private:
		bool isActive_ = false;				// 退場演出中か
		bool hasStarted_ = false;				// 撃破ではなく退場で削除されたかの判定用
		float elapsedTime_ = 0.0f;			// 退場開始からの時間
		float duration_ = 1.2f;				// 退場に要する時間
		Vector3 startScale_{ 1.0f, 1.0f, 1.0f };	// 開始時のモデルサイズ
		Vector3 startShadowScale_{ 1.0f, 1.0f, 1.0f };	// 開始時の影サイズ
		float startHeight_ = 0.0f;			// 開始時の高さ
	};
}
