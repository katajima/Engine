#pragma once
#include "EnemyCrowdData.h"
#include "EnemyCrowdLayerSystem.h"
#include "EnemyFlockingSystem.h"
#include "EnemyFlowFieldSystem.h"
#include "EnemyCrowdPatternSystem.h"
#include <cstdint>
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 敵の隊形生成と、分割された群衆AI機能の実行順を管理するシステム
	/// </summary>
	class EnemyCrowdSystem {
	public:
		// 初期化
		void Initialize();

		// 敵一覧からグループごとの隊形スロットを作成する
		void Update(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos,
			float targetRotateY,
			float dt
		);

		// 指定した敵に割り当てられた隊形スロットを取得
		const CrowdSlot* FindSlot(BaseEnemy* enemy) const;
		// 指定した敵のフロッキング計算済み移動目標を取得
		const EnemyFlockingSteering* FindSteering(BaseEnemy* enemy) const;

	private:
		// 1グループ分の隊形スロットを割り当てる
		void AssignGroupSlots(
			const std::vector<BaseEnemy*>& groupEnemies,
			int groupId,
			const Vector3& targetPos,
			const Vector3& forward,
			const Vector3& right,
			uint32_t groupOrder
		);

		// レイヤー、流れ方向、フロッキングを順に計算して移動結果を作る
		void BuildFlockingSteering(
			const std::vector<BaseEnemy*>& enemies,
			const Vector3& targetPos
		);

	private:
		std::vector<CrowdSlot> slots_;
		std::vector<EnemyFlockingSteering> steerings_;

		float frontDistance_ = 8.0f;		// ターゲットから隊形先頭までの距離
		float rowSpacing_ = 3.0f;			// 前後の間隔
		float columnSpacing_ = 2.6f;		// 左右の間隔
		uint32_t columnCount_ = 3;			// 1列に並ぶ敵数

		EnemyCrowdLayerSystem layerSystem_;		// 攻撃・接近・待機の役割決定
		EnemyFlowFieldSystem flowFieldSystem_;	// 隊形目標への基本進行方向
		EnemyFlockingSystem flockingSystem_;	// 群れ行動を反映した最終移動目標
		EnemyCrowdPatternSystem patternSystem_;	// 群衆タイプ別の隊形目標生成
	};
}
