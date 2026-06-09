#pragma once
#include "EnemyAttackRequest.h"
#include "EnemyCrowdBehaviorData.h"
#include <vector>
#include <cstdint>


namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 敵から出された攻撃要求を整理し、同時攻撃数や群衆戦術に応じて許可を出す。
	/// </summary>
	class EnemyAttackRequestSystem {
	public:
		// 攻撃要求管理を初期化する
		void Initialize();

		// 敵一覧の攻撃要求を更新し、攻撃リングを割り当てる
		void Update(const std::vector<BaseEnemy*>& enemies, float dt);

	private:
		// 攻撃許可判定に使う敵と要求の組
		struct AttackCandidate {
			BaseEnemy* enemy = nullptr;				// 候補になっている敵
			EnemyAttackRequest* request = nullptr;	// 敵が保持している攻撃要求
		};

		// 群衆タイプと設定から、攻撃許可の出し方を決める
		EnemyCrowdAttackStyle ResolveAttackStyle(const BaseEnemy* enemy, size_t groupSize) const;
		// 個人順で攻撃許可を出す
		void AssignIndividual(std::vector<AttackCandidate>& candidates, uint32_t& attackCount, uint32_t& approachCount);
		// 同じ群衆から複数体へ同時に攻撃許可を出す
		void AssignGroupVolley(std::vector<AttackCandidate>& candidates, uint32_t& attackCount);
		// 統率者だけを攻撃役へ回し、他メンバーは接近・待機へ回す
		void AssignLeaderOnly(std::vector<AttackCandidate>& candidates, uint32_t& attackCount);
		// 包囲がある程度成立した時だけ複数体へ攻撃許可を出す
		void AssignSurroundThenAttack(std::vector<AttackCandidate>& candidates, uint32_t& attackCount, uint32_t& approachCount);

		// インターバルタイマーを進める。攻撃許可を出した後、次の許可まで少し間隔を空けるために使う
		void UpdateIntervalTimer(float dt);
	private:
		// 攻撃要求に付ける連番
		uint32_t requestCounter_ = 1;	

		// 同時に攻撃できる敵数
		uint32_t maxAttackCount_ = 2;

		// 攻撃待ちの中でも前に詰める敵数
		uint32_t approachRingCount_ = 2;

		// 攻撃許可の間隔
		float attackInterval_ = 0.1f;
		float intervalTimer_ = 0.0f;	// 次の攻撃許可までの残り時間

		// 攻撃後、同じ敵が再攻撃できない時間
		float reAttackCooldown_ = 3.0f;

		// 距離差がこの値以内なら要求順を優先
		float sameDistanceRange_ = 1.0f;

		uint32_t maxGroupAttackCount_ = 4;	// 一斉攻撃で同時に許可する最大数
		uint32_t surroundReadyCount_ = 3;	// 包囲型で攻撃開始とみなす最低人数
		float surroundReadyDistance_ = 12.0f;	// 包囲成立判定に使う距離
	};
}
