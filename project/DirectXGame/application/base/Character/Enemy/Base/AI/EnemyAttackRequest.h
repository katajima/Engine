#pragma once
#include <cstdint>

namespace Character {

	// 敵がターゲットとの距離に応じて取る攻撃待機リング
	enum class EnemyAttackRing {
		None,		// 未分類
		TooClose,	// 近すぎるため攻撃待ちへ戻す
		Attack,		// 攻撃許可を受けた状態
		Standby,	// 密集回避や順番待ちで待機する状態
		Approach,	// 攻撃候補として接近する状態
	};

	/// <summary>
	/// 1体の敵が持つ攻撃要求状態。攻撃許可、距離、再攻撃クールタイムを保持する。
	/// </summary>
	class EnemyAttackRequest {
	public:
		// 攻撃要求を有効化する
		void Activate() {
			isActive = true;
		}

		// 攻撃要求を無効化し、許可状態と分類番号をリセットする
		void Deactivate() {
			isActive = false;
			isGranted = false;
			number = 0;
			ring = EnemyAttackRing::None;
		}

		// 攻撃許可を与える
		void Grant() {
			isGranted = true;
		}

		// 攻撃許可を取り消す
		void Deny() {
			isGranted = false;
		}

		// ターゲットまでの距離を記録する
		void SetDistance(float value) {
			distance = value;
		}

		// ターゲットまでの距離を取得する
		float GetDistance() const {
			return distance;
		}

		// 攻撃リングを設定する
		void SetRing(EnemyAttackRing value) {
			ring = value;
		}

		// 現在の攻撃リングを取得する
		EnemyAttackRing GetRing() const {
			return ring;
		}

		// 攻撃後の再攻撃クールタイムを開始する
		void StartCooldown(float time) {
			cooldownTimer = time;
		}

		// クールタイムを経過時間分だけ進める
		void UpdateCooldown(float dt) {
			if (cooldownTimer > 0.0f) {
				cooldownTimer -= dt;
				if (cooldownTimer < 0.0f) {
					cooldownTimer = 0.0f;
				}
			}
		}

		// 再攻撃クールタイム中かを返す
		bool IsCooldown() const {
			return cooldownTimer > 0.0f;
		}

		// 攻撃要求が有効かを返す
		bool GetIsActive() const { return isActive; }
		// 攻撃許可を受けているかを返す
		bool GetIsGranted() const { return isGranted; }

		// 要求順を比較するための連番を取得する
		uint32_t GetNumber() const { return number; }
		// 要求順を比較するための連番を設定する
		void SetNumber(uint32_t value) { number = value; }

	private:
		float distance = 0.0f;			// ターゲットまでの距離
		float cooldownTimer = 0.0f;		// 再攻撃までの残り時間

		EnemyAttackRing ring = EnemyAttackRing::None;	// 現在の攻撃リング

		uint32_t number = 0;				// 攻撃要求を出した順番

		bool isActive = false;			// 攻撃要求が有効か
		bool isGranted = false;			// 攻撃許可を受けているか
	};
}
