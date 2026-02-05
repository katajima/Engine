#include "ComboHitBox.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {
#pragma region ComboHitBox

	// 開始
	void ComboHitBox::Enter(BaseCharacter* owner) {
		// ヒットボックスシステムを渡す
		hitBoxSystem_ = owner->GetAttackController()->GetHitBoxSystem();
		// ジャンプシステムを渡す
		jumpSystem_ = owner->GetMoveComponent()->GetJumpSystem();
	}

	// 更新
	void ComboHitBox::Update(const Engine::Input& input, float timer, float dt) {


		for (auto& coll : collData_) {
			coll.reactionData.GetKnockbackData().SetNormal(direction_);
		}


		switch (data_.spawnType_)
		{
		case HitBox::SpawnType::kOnTime: // 時間経過で
			if (timer >= data_.hitBpxWindowStart_) {
				if (!isPopHitBox_) {
					hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
					isPopHitBox_ = true;
				}
			}
			break;
		case HitBox::SpawnType::kOnGround: // 着地したら
			if (jumpSystem_->GetIsLanding()) {
				timer_ += dt;
				if (timer >= data_.hitBpxWindowStart_) {
					if (!isPopHitBox_) {
						hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
						isPopHitBox_ = true;
					}
				}
			}
			break;
		case HitBox::SpawnType::kOnAir:
			break;
		case HitBox::SpawnType::kOnButtonRelease: // ボタンを離したら
			if (button_.IsReleased(input)) {
				if (!isPopHitBox_) {
					hitBoxSystem_->AddHitBox(data_.hitBoxUseType_, collData_, useHitBox_, data_.lifeTime_, data_.dependenceType_, data_.offset_, perent_);
					isPopHitBox_ = true;
				}
			}
			break;
		default:
			break;
		}



	}

	// 終了
	void ComboHitBox::Exit() {
		isPopHitBox_ = false;
		timer_ = 0.0f;
	}

	void ComboHitBox::AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlovalData& combo) {
		HitBox::CollData data = hitBoxData;

		// リアクションデータ
		data.reactionData.GetDamageData().GetOne().damage = combo.reaction.damage;
		data.reactionData.GetKnockbackData().GetData().power_ = combo.reaction.knockbackPower;
		data.reactionData.GetKnockbackData().GetData().verticalBoost_ = combo.reaction.knockbackPowerY;
		data.reactionData.GetKnockbackData().GetData().duration_ = combo.reaction.knockbackDuration_;
		data.reactionData.GetKnockbackData().GetData().isVerticalBoost_ = combo.reaction.isVerticalBoost_;

		collData_.push_back(data);
	};

#pragma endregion // コンボヒットボックス
};
