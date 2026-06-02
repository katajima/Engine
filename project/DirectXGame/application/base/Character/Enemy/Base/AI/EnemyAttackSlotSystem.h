#pragma once
#include <vector>
#include <array>
#include <unordered_map>
#include <DirectXGame/engine/struct/Vector3.h>

namespace Character {
	class BaseEnemy;

	struct AttackSlot {
		Vector3 position{};
		BaseEnemy* owner = nullptr;
		bool occupied = false;
	};

	class EnemyAttackSlotSystem {
	public:
		void Initialize();

		void Update(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY,
			float dt
		);

		const AttackSlot* FindSlot(BaseEnemy* enemy) const;
		// デバッグ描画など読み取り用途に、全攻撃位置を公開する
		const auto& GetSlots() const { return slots_; }

	private:
		void CreateSlots(const Vector3& targetPos, float targetRotateY);
		void AssignSlots(const std::vector<BaseEnemy*>& enemies, float dt);
		void ClearOwners();
		void CleanupSlotHolds(const std::vector<BaseEnemy*>& enemies);

	private:
		static constexpr uint32_t kSlotCount = 8;
		struct SlotHoldState {
			Vector3 position{};
			uint32_t index = 0;
			float timer = 0.0f;
			bool hasSlot = false;
		};

		std::array<AttackSlot, kSlotCount> slots_{};
		std::unordered_map<BaseEnemy*, SlotHoldState> slotHolds_;

		float slotRadius_ = 8.0f;
		float slotChangeCooldown_ = 0.35f;	// 攻撃位置を切り替えるまでの待ち時間
		float slotSwitchDistance_ = 1.8f;	// この距離以上ずれた時だけ切り替え対象にする
	};
}
