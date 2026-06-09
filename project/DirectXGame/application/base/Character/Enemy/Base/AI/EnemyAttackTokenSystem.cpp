#include "EnemyAttackTokenSystem.h"

namespace Character {

	void EnemyAttackTokenSystem::Initialize() {
		// 起動時やリセット時に、使用中トークン数を空にする
		BeginFrame();
	}

	void EnemyAttackTokenSystem::BeginFrame() {
		// 攻撃許可の再計算ごとに、今回分の使用数だけをリセットする
		grantedCount_ = 0;
		approachCount_ = 0;
	}

	void EnemyAttackTokenSystem::SetLimits(uint32_t individualMax, uint32_t groupMax, uint32_t approachMax) {
		// 外部調整用に上限をまとめて設定できるようにする
		individualAttackLimit_ = individualMax;
		groupAttackLimit_ = groupMax;
		approachLimit_ = approachMax;
	}

	bool EnemyAttackTokenSystem::TryGrantAttack(EnemyAttackRequest* request, uint32_t tokenLimit) {
		if (!request) {
			// 攻撃要求が無い敵にはトークンを渡せない
			return false;
		}

		if (grantedCount_ >= tokenLimit) {
			// 今回使える攻撃トークンが残っていない
			return false;
		}

		// 攻撃リングへ入れ、実際に攻撃開始してよい状態へする
		request->SetRing(EnemyAttackRing::Attack);
		request->Grant();
		grantedCount_++;
		return true;
	}

	bool EnemyAttackTokenSystem::TryAssignApproach(EnemyAttackRequest* request) {
		if (!request) {
			// 攻撃要求が無い敵は接近リングにも入れない
			return false;
		}

		if (approachCount_ >= approachLimit_) {
			// 接近役が多すぎると密集するため、上限を超えたら待機へ回す
			return false;
		}

		// 次の攻撃候補として、プレイヤー側へ詰める役にする
		request->SetRing(EnemyAttackRing::Approach);
		approachCount_++;
		return true;
	}

	void EnemyAttackTokenSystem::AssignStandby(EnemyAttackRequest* request) const {
		if (!request) {
			return;
		}

		// 攻撃枠にも接近枠にも入らない敵は、密集を避けるため待機へ送る
		request->SetRing(EnemyAttackRing::Standby);
	}
}
