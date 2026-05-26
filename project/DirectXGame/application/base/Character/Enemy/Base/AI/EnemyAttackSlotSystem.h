#pragma once
#include <vector>
#include <array>
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
			float targetRotateY
		);

		const AttackSlot* FindSlot(BaseEnemy* enemy) const;
		// デバッグ描画など読み取り用途に、全攻撃位置を公開する
		const auto& GetSlots() const { return slots_; }

	private:
		void CreateSlots(const Vector3& targetPos, float targetRotateY);
		void AssignSlots(const std::vector<BaseEnemy*>& enemies);
		void ClearOwners();

	private:
		static constexpr uint32_t kSlotCount = 8;

		std::array<AttackSlot, kSlotCount> slots_{};

		float slotRadius_ = 8.0f;
	};
}
