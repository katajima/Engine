#include "ComboHitBox.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"

namespace Combo {
#pragma region ComboHitBox

	// 開始
	void ComboHitBox::Enter(Character::BaseCharacter* owner, Type type) {
		this->type = type;
		this->owner = owner;
		// ヒットボックスシステムを渡す
		hitBoxSystem = owner->GetHitBoxSystem();
		// 移動システムを渡す
		movementComponent = owner->GetMoveComponent();

		// 常時なら
		if (data_.lifetimeType == HitBox::LifetimeType::kInfinite) {
			hitBoxSystem->AddHitBox(id, data_.useType, owner, collData_, data_.dependenceType, data_.parentOffset, data_.useContactRecord, perent);
			hitBox = hitBoxSystem->GetHitBoxInstance(id);
			hitBox->Disable(Vector4{ 1,1,1,0.0f });
			hitBox->ClearContactRecord();
		}
	}

	// 更新
	void ComboHitBox::Update(const Character::CharacterContext& ctx, float timer) {
		// ノックバック方向
		collData_.reactionData.normal = direction;

		// 近距離か複合なら
		if (Type::kMelle == type || Type::kMix == type) {
			// 一時的なコライダーなら
			if (data_.lifetimeType == HitBox::LifetimeType::kTimed && !isPopHitBox_) {
				// 出現方法によっての処理
				switch (data_.spawnType)
				{
				case HitBox::SpawnType::kOnTime: // 時間経過で
					if (timer >= data_.windowStart) {
						hitBoxSystem->AddLifeTimeHitBox(data_.useType, owner, collData_,  data_.lifeTime, data_.dependenceType, data_.parentOffset, data_.useContactRecord, perent);
						isPopHitBox_ = true;
					}
					break;
				case HitBox::SpawnType::kOnGround: // 着地したら
					if (movementComponent->GetIsLanding()) {
						hitBoxSystem->AddLifeTimeHitBox(data_.useType, owner, collData_, data_.lifeTime, data_.dependenceType, data_.parentOffset, data_.useContactRecord, perent);
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
			else if (data_.lifetimeType == HitBox::LifetimeType::kInfinite) {
				if (hitBox) {
					if (timer >= data_.windowStart && timer <= data_.lifeTime + data_.lifeTime) {
						hitBox->Enable();
					}
					else {
						hitBox->Disable(Vector4{ 1,1,1,0.0f });
					}
				}
			}
		}
	}

	// 終了
	void ComboHitBox::Exit() {
		isPopHitBox_ = false;
		hitBoxSystem->Clear();
		if (hitBox) {
			hitBox->Disable();
			hitBox->ClearContactRecord();
		}
	}

	void ComboHitBox::AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlobalData& combo) {
		HitBox::CollData data = hitBoxData;
		// リアクションデータ
		data.reactionData = combo.hitReaction;
		data.hitBoxData = combo.hitBox;
		collData_ = data;
	};

#pragma endregion // コンボヒットボックス
};
