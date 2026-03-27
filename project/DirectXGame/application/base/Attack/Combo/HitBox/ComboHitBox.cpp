#include "ComboHitBox.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {
#pragma region ComboHitBox

	// 開始
	void ComboHitBox::Enter(Character::BaseCharacter* owner, Type type) {
		this->type = type;
		// ヒットボックスシステムを渡す
		hitBoxSystem = owner->GetAttackController()->GetHitBoxSystem();
		// 移動システムを渡す
		movementComponent = owner->GetMoveComponent();

		// 常時なら
		if (data_.lifetimeType == HitBox::LifetimeType::kInfinite) {
			hitBoxSystem->AddHitBox(data_.hitBoxUseType, collData_, useHitBox_, data_.dependenceType, data_.offset, perent);
		}
	}

	// 更新
	void ComboHitBox::Update(const Character::CharacterContext& ctx, float timer) {
		// ノックバック方向
		for (auto& coll : collData_) {
			coll.reactionData.GetKnockbackData().SetNormal(direction);
		}

		// 近距離か複合なら
		if (Type::kMelle == type || Type::kMix == type) {
			// 一時的なコライダーなら
			if (data_.lifetimeType == HitBox::LifetimeType::kTimed && !isPopHitBox_) {
				// 出現方法によっての処理
				switch (data_.spawnType)
				{
				case HitBox::SpawnType::kOnTime: // 時間経過で
					if (timer >= data_.hitBpxWindowStart) {
						hitBoxSystem->AddLifeTimeHitBox(data_.hitBoxUseType, collData_, useHitBox_, data_.lifeTime, data_.dependenceType, data_.offset, perent);
						isPopHitBox_ = true;
					}
					break;
				case HitBox::SpawnType::kOnGround: // 着地したら
					if (movementComponent->GetIsLanding()) {
						hitBoxSystem->AddLifeTimeHitBox(data_.hitBoxUseType, collData_, useHitBox_, data_.lifeTime, data_.dependenceType, data_.offset, perent);
						isPopHitBox_ = true;
					}
					break;
				case HitBox::SpawnType::kOnAir:
					break;
				case HitBox::SpawnType::kOnButtonRelease: // ボタンを離したら
					break;
				default:
					break;
				}
			}
			// 常時なら
			else if(data_.lifetimeType == HitBox::LifetimeType::kInfinite){
				if (timer >= data_.hitBpxWindowStart) {
				}
			}
		}
	}

	// 終了
	void ComboHitBox::Exit() {
		isPopHitBox_ = false;
		hitBoxSystem->Clear();
	}

	void ComboHitBox::AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlobalData& combo) {
		HitBox::CollData data = hitBoxData;

		// リアクションデータ
		data.reactionData.GetDamageData().GetOne().damage = combo.reaction.damage;
		data.reactionData.GetKnockbackData().GetData().power_ = combo.reaction.knockbackPower;
		data.reactionData.GetKnockbackData().GetData().verticalBoost_ = combo.reaction.knockbackPowerY;
		data.reactionData.GetKnockbackData().GetData().duration_ = combo.reaction.knockbackDuration;
		data.reactionData.GetKnockbackData().GetData().isVerticalBoost_ = combo.reaction.isVerticalBoost;

		collData_.push_back(data);
	};

#pragma endregion // コンボヒットボックス
};
