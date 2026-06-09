#pragma once
#include "EnemyAttackCandidate.h"
#include "EnemyAttackTokenSystem.h"
#include "EnemyCombatDirector.h"
#include <cstdint>
#include <vector>

namespace Character {
	class BaseEnemy;

	/// <summary>
	/// 敵から出された攻撃要求を集め、Combat DirectorとAttack Tokenへ攻撃許可判断を渡す。
	/// 候補収集とクールタイム管理だけを担当し、攻撃方式の判断はEnemyCombatDirectorに分離する。
	/// </summary>
	class EnemyAttackRequestSystem {
	public:
		// 攻撃要求管理を初期化する
		void Initialize();

		// 敵一覧の攻撃要求を更新し、攻撃リングを割り当てる
		void Update(const std::vector<BaseEnemy*>& enemies, float dt);

	private:
		// インターバルタイマーを進める。攻撃許可を出した後、次の許可まで少し間隔を空けるために使う
		void UpdateIntervalTimer(float dt);

	private:
		uint32_t requestCounter_ = 1;	// 攻撃要求に付ける連番

		uint32_t maxAttackCount_ = 2;	// 個人攻撃で同時に攻撃できる敵数
		uint32_t maxGroupAttackCount_ = 4;	// 一斉攻撃で同時に許可する最大数
		uint32_t approachRingCount_ = 2;	// 攻撃待ちの中でも前に詰める敵数

		float attackInterval_ = 0.1f;	// 攻撃許可を更新する間隔
		float intervalTimer_ = 0.0f;		// 次の攻撃許可までの残り時間

		float reAttackCooldown_ = 3.0f;	// 攻撃後、同じ敵が再攻撃できない時間

		EnemyAttackTokenSystem attackTokenSystem_;	// 攻撃許可トークンの使用数と上限を管理する
		EnemyCombatDirector combatDirector_;		// 群衆タイプごとの攻撃方式を判断する
	};
}
