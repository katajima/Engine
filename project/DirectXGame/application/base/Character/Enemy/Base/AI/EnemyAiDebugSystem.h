#pragma once
#include <vector>

namespace Engine {
	class LineCommon;
}

namespace Character {
	class BaseEnemy;
	class EnemyAiSystem;

	/// <summary>
	/// 敵AIが選択した移動先や役割を可視化するデバッグ専用システム。
	/// </summary>
	class EnemyAiDebugSystem {
	public:
		void Initialize(Engine::LineCommon* lineCommon);
		void Update(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem);

	private:
#ifdef _DEBUG
		void DrawControlPanel(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem);
		void DrawWorldDebug(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem);
#endif

	private:
		Engine::LineCommon* lineCommon_ = nullptr;
		bool isVisible_ = false;			// デバッグ表示全体の有効化
		bool showMoveTarget_ = true;		// フロッキング合成後の最終移動目標
		bool showFormationSlot_ = false;	// 群衆パターンが作った隊形位置
		bool showFlowDirection_ = false;	// 流れ制御が示す進行方向
		bool showAttackSlots_ = true;		// プレイヤー周囲の攻撃位置
		bool showEnemyDetails_ = false;		// 敵ごとの判断一覧
	};
}
