#pragma once
#include "EnemyAttackRequest.h"
#include <cstdint>

namespace Character {

	/// <summary>
	/// 攻撃許可トークンを管理するシステム。
	/// 同時攻撃数と接近待ち数をここで制限し、Combat Directorから使いやすい形に分離する。
	/// </summary>
	class EnemyAttackTokenSystem {
	public:
		// トークン数と接近数を初期状態へ戻す
		void Initialize();

		// 1回の攻撃許可更新を始める前に、今回分の使用数をリセットする
		void BeginFrame();

		// 攻撃トークン数と接近リング数の上限を設定する
		void SetLimits(uint32_t individualMax, uint32_t groupMax, uint32_t approachMax);

		// 攻撃トークンを1つ消費し、敵へ攻撃許可を出す
		bool TryGrantAttack(EnemyAttackRequest* request, uint32_t tokenLimit);

		// 接近リングの枠を1つ消費し、敵を次の攻撃候補として前に出す
		bool TryAssignApproach(EnemyAttackRequest* request);

		// 攻撃・接近枠に入らない敵を待機リングへ送る
		void AssignStandby(EnemyAttackRequest* request) const;

		// 個人攻撃で使う同時攻撃上限を返す
		uint32_t GetIndividualAttackLimit() const { return individualAttackLimit_; }
		// 一斉攻撃で使う同時攻撃上限を返す
		uint32_t GetGroupAttackLimit() const { return groupAttackLimit_; }
		// 接近リングの上限を返す
		uint32_t GetApproachLimit() const { return approachLimit_; }
		// 今回すでに攻撃許可を出した数を返す
		uint32_t GetGrantedCount() const { return grantedCount_; }
		// 今回すでに接近リングへ送った数を返す
		uint32_t GetApproachCount() const { return approachCount_; }

	private:
		uint32_t individualAttackLimit_ = 2;	// 個人攻撃時に同時に攻撃できる最大数
		uint32_t groupAttackLimit_ = 4;		// 一斉攻撃時に同時に攻撃できる最大数
		uint32_t approachLimit_ = 2;			// 攻撃待ちとして前に出せる最大数

		uint32_t grantedCount_ = 0;			// 今回の更新で攻撃許可を出した数
		uint32_t approachCount_ = 0;			// 今回の更新で接近リングへ送った数
	};
}
