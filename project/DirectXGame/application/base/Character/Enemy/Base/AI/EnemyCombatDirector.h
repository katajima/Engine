#pragma once
#include "EnemyAttackCandidate.h"
#include "EnemyAttackTokenSystem.h"
#include "EnemyCrowdBehaviorData.h"
#include <cstdint>
#include <vector>

namespace Character {

	/// <summary>
	/// 群衆ごとの攻撃方式を決め、Attack Tokenを使って攻撃役・接近役・待機役を割り当てる。
	/// 攻撃の作戦判断をEnemyAttackRequestSystemから分離するためのクラス。
	/// </summary>
	class EnemyCombatDirector {
	public:
		/// <summary>
		/// Combat Directorの内部パラメータを初期状態にする
		/// </summary>
		void Initialize();

		/// <summary>
		/// 攻撃候補を群衆ごとに整理し、群衆タイプに応じて攻撃トークンを配る
		/// </summary>
		void Direct(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem);

	private:
		/// <summary>
		/// 群衆タイプと設定から、攻撃許可の出し方を決める
		/// </summary>
		EnemyCrowdAttackStyle ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const;

		/// <summary>
		/// 個人順で攻撃許可を出す
		/// </summary>
		void AssignIndividual(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const;

		/// <summary>
		/// 同じ群衆から複数体へ同時に攻撃許可を出す
		/// </summary>
		void AssignGroupVolley(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const;

		/// <summary>
		/// 統率者だけを攻撃役へ回し、他メンバーは接近・待機へ回す
		/// </summary>
		void AssignLeaderOnly(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const;

		/// <summary>
		/// 包囲がある程度成立した時だけ複数体へ攻撃許可を出す
		/// </summary>
		void AssignSurroundThenAttack(std::vector<EnemyAttackCandidate>& candidates, EnemyAttackTokenSystem& tokenSystem) const;

		float sameDistanceRange_ = 1.0f;			// 距離差がこの値以内なら要求順を優先する
		uint32_t surroundReadyCount_ = 3;		// 包囲型で攻撃開始とみなす最低人数
		float surroundReadyDistance_ = 12.0f;	// 包囲成立判定に使う距離
	};
}
