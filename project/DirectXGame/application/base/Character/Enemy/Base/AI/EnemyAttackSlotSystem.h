#pragma once
#include <vector>
#include <array>
#include <unordered_map>
#include <DirectXGame/engine/struct/Vector3.h>

namespace Character {
	class BaseEnemy;

	// プレイヤー周囲に作られる攻撃位置と、その位置を担当する敵
	struct AttackSlot {
		Vector3 position{};			// 攻撃時に敵が目指すワールド座標
		BaseEnemy* owner = nullptr;	// このスロットを確保している敵
		bool occupied = false;		// 今フレーム使用中か
	};

	/// <summary>
	/// 複数の敵が同じ攻撃位置へ重ならないよう、プレイヤー周囲のスロットを割り当てる。
	/// </summary>
	class EnemyAttackSlotSystem {
	public:
		/// <summary>
		/// スロット状態を初期化する
		/// </summary>
		void Initialize();

		// ターゲット位置を基準に攻撃スロットを作り、敵へ割り当てる
		void Update(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY,
			float dt
		);

		/// <summary>
		/// 指定した敵が確保している攻撃スロットを取得する
		/// </summary>
		const AttackSlot* FindSlot(BaseEnemy* enemy) const;
		/// <summary>
		/// デバッグ描画など読み取り用途に、全攻撃位置を公開する
		/// </summary>
		const auto& GetSlots() const { return slots_; }

	private:
		/// <summary>
		/// ターゲットの周囲に円形の攻撃スロットを作成する
		/// </summary>
		void CreateSlots(const Vector3& targetPos, float targetRotateY);
		/// <summary>
		/// 各敵に最も近い未使用スロットを割り当てる
		/// </summary>
		void AssignSlots(const std::vector<BaseEnemy*>& enemies, float dt);
		/// <summary>
		/// 今フレームの割り当て情報をリセットする
		/// </summary>
		void ClearOwners();
		/// <summary>
		/// 消滅した敵の保持情報を削除する
		/// </summary>
		void CleanupSlotHolds(const std::vector<BaseEnemy*>& enemies);

	private:
		// プレイヤー周囲に作る攻撃スロット数
		static constexpr uint32_t kSlotCount = 8;
		// スロット切り替えを滑らかにするために前回の割り当てを保持する
		struct SlotHoldState {
			Vector3 position{};	// 保持している攻撃位置
			uint32_t index = 0;	// 保持しているスロット番号
			float timer = 0.0f;	// 切り替えを抑える残り時間
			bool hasSlot = false;	// 有効な保持情報があるか
		};

		std::array<AttackSlot, kSlotCount> slots_{};					// 今フレームの攻撃スロット一覧
		std::unordered_map<BaseEnemy*, SlotHoldState> slotHolds_;	// 敵ごとの前回スロット保持情報

		float slotRadius_ = 8.0f;				// ターゲットから攻撃位置までの距離
		float slotChangeCooldown_ = 0.35f;	// 攻撃位置を切り替えるまでの待ち時間
		float slotSwitchDistance_ = 1.8f;	// この距離以上ずれた時だけ切り替え対象にする
	};
}
