#include "SmallMeleeEnemySubState.h"
#include "SmallMeleeEnemy.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
namespace Character {
	void SmallMeleeAttackReadySubState::Enter() {
		timer_ = 0.0f;

		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		// Ready開始時の方向を一旦初期化
		Vector3 toTarget = Subtract(enemy->GetTargetPos(), enemy->GetWorldTransform().translate_);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			dire_ = toTarget.Normalize();
		}
		else {
			dire_ = enemy->TargetDirection();
			dire_.y = 0.0f;
			if (dire_.Length() > 0.001f) {
				dire_ = dire_.Normalize();
			}
		}
	}

	void SmallMeleeAttackReadySubState::Update(float deltaTime) {
		timer_ += deltaTime;

		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		// 毎フレームターゲット方向を取得
		Vector3 toTarget = Subtract(enemy->GetTargetPos(), enemy->GetWorldTransform().translate_);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			Vector3 targetDire = toTarget.Normalize();

			// ここは即向きでもいいが、本当は角度補間推奨
			enemy->GetWorldTransform().rotate_.y = std::atan2(targetDire.x, targetDire.z);

			// Swingで使う方向は Ready の最後まで更新しておく
			dire_ = targetDire;
		}

		MoveRequest request{};
		request.priority = 1;
		request.direction = dire_;

		const float distance = enemy->GetTargetDistance();

		// 近すぎるときだけ軽く引く
		if (distance <= 5.5f) {
			request.velocity = dire_ * -4.0f * deltaTime;
		}
		// 攻撃準備距離なら基本停止
		else if (distance <= 8.0f) {
			request.velocity = Vector3(0.0f, 0.0f, 0.0f);
		}
		// 遠すぎるなら少しだけ詰める
		else {
			request.velocity = dire_ * 2.0f * deltaTime;
		}

		enemy->GetMoveComponent()->GetMoveRequestSystem()->SetRequest(request);

		if (timer_ >= readyTime_) {
			fsm_->ChangeState(AttackSubState::Swing);
		}
	}

	void SmallMeleeAttackSwingSubState::Enter() {
		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		timer_ = 0.0f;

		// Readyで最終的に合わせた方向を使う
		Vector3 toTarget = Subtract(enemy->GetTargetPos(), enemy->GetWorldTransform().translate_);
		toTarget.y = 0.0f;

		if (toTarget.Length() > 0.001f) {
			dire_ = toTarget.Normalize();
		}
		else {
			dire_ = enemy->TargetDirection();
			dire_.y = 0.0f;
			if (dire_.Length() > 0.001f) {
				dire_ = dire_.Normalize();
			}
		}

		// 向きを固定
		enemy->GetWorldTransform().rotate_.y = std::atan2(dire_.x, dire_.z);

		HitBox::CollData data{};
		data.hitBoxData.isEneble = true;
		data.hitBoxData.isLine = true;
		data.hitBoxData.tag = CollisionTag::EnemyAttack;
		data.hitBoxData.layer = CollisionLayer::EnemyAttack;
		data.hitBoxData.mask = CollisionLayer::Player;
		data.hitBoxData.colliderSize = { 1.0f, 2.0f, 1.0f };
		data.reactionData.damageData.GetOne().SetDamage(10.0f);

		data.hitBoxData.useType = HitBox::UseType::kEnemy;
		data.hitBoxData.lifeTime = swingTime_;
		data.hitBoxData.dependenceType = HitBox::ParentType::kParent;
		data.hitBoxData.offset = {  };
		data.hitBoxData.useContactRecord = true;

		// ヒットボックス寿命は swingTime_ と揃える
		enemy->GetHitBoxSystem()->AddLifeTimeHitBox(
			enemy,
			data,
			&enemy->GetWorldTransform()
		);
	}

	void SmallMeleeAttackSwingSubState::Update(float deltaTime) {
		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		timer_ += deltaTime;

		MoveRequest request{};
		request.priority = 1;
		request.direction = dire_;
		request.velocity = dire_ * 20.0f * deltaTime;
		enemy->GetMoveComponent()->GetMoveRequestSystem()->SetRequest(request);

		if (timer_ >= swingTime_) {
			fsm_->ChangeState(AttackSubState::End);
		}
	}

	void SmallMeleeAttackSwingSubState::Exit() {
	}




	void SmallMeleeAttackEndSubState::Enter() {
		timer_ = 0.0f;

		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		startRotateY_ = enemy->GetWorldTransform().rotate_.y;

		Vector3 toPlayer = Subtract(enemy->GetTargetPos(), enemy->GetWorldTransform().translate_);
		toPlayer.y = 0.0f;

		if (toPlayer.Length() > 0.001f) {
			toPlayer = toPlayer.Normalize();
			targetRotateY_ = std::atan2(toPlayer.x, toPlayer.z);
		}
		else {
			targetRotateY_ = startRotateY_;
		}

		// 最短差分を保存
		rotateDiffY_ = targetRotateY_ - startRotateY_;
		rotateDiffY_ = std::atan2(std::sin(rotateDiffY_), std::cos(rotateDiffY_));
	}

	void SmallMeleeAttackEndSubState::Update(float deltaTime) {
		BaseEnemy* enemy = dynamic_cast<BaseEnemy*>(character_);
		if (!enemy) {
			return;
		}

		timer_ += deltaTime;

		float t = std::clamp(timer_ / endTime_, 0.0f, 1.0f);

		// 必要なら ease をかける
		float easedT = t * t * (3.0f - 2.0f * t);

		float newY = startRotateY_ + rotateDiffY_ * easedT;
		enemy->GetWorldTransform().rotate_.y = newY;

		if (timer_ >= endTime_) {
			fsm_->SetFinished(true);
		}
	}

}
