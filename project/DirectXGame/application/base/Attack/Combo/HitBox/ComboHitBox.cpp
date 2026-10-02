#include "ComboHitBox.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include <algorithm>

namespace Combo {
#pragma region ComboHitBox

	// 開始
	void ComboHitBox::Enter(Character::BaseCharacter* owner, Type type) {
		// 使用者と攻撃種別を保持し、以降のヒットボックス更新で参照する
		this->type = type;
		this->owner = owner;
		// ヒットボックスシステムを渡す
		hitBoxSystem = owner->GetHitBoxSystem();
		// ノードごとの一時判定を他の攻撃と分離して終了できるようにする。
		hitBoxOwnerId_ = hitBoxSystem->AllocateOwnerId();
		// ノード再利用時に判定波の進行状態を初期化する。
		spawnedCount_ = 0;
		nextSpawnTime_ = collData_.hitBoxData.windowStart;
		spawnStarted_ = false;
		// 移動システムを渡す
		movementComponent = owner->GetMoveComponent();

		// 常時なら
		if (collData_.hitBoxData.lifetimeType == HitBox::LifetimeType::kInfinite) {
			// 常時型は開始時に生成しておき、受付時間外は無効化して使い回す
			hitBoxSystem->AddHitBox(id, owner, collData_, perent);
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
			if (collData_.hitBoxData.lifetimeType == HitBox::LifetimeType::kTimed &&
				spawnedCount_ < (std::max)(1u, collData_.hitBoxData.spawnCount)) {
				const bool intervalReady = spawnStarted_ && timer >= nextSpawnTime_;
				bool firstSpawnReady = false;
				// 一時型は生成条件を満たした瞬間だけヒットボックスを追加する
				// 出現方法によっての処理
				switch (collData_.hitBoxData.spawnType)
				{
				case HitBox::SpawnType::kOnTime: // 時間経過で
					// 指定された開始時間を過ぎたら生成する
					firstSpawnReady = !spawnStarted_ && timer >= collData_.hitBoxData.windowStart;
					break;
				case HitBox::SpawnType::kOnGround: // 着地したら
					// 空中攻撃などで、着地した瞬間に発生する攻撃用
					firstSpawnReady = !spawnStarted_ && movementComponent && movementComponent->GetIsLanding();
					break;
				case HitBox::SpawnType::kOnAir:
					firstSpawnReady = !spawnStarted_ && movementComponent && !movementComponent->GetIsLanding();
					break;
				case HitBox::SpawnType::kOnButtonRelease: // ボタンを離したら
					firstSpawnReady = !spawnStarted_ &&
						(ctx.inputData.lightAttackReleased || ctx.inputData.heavyAttackReleased ||
							ctx.inputData.skillReleased || ctx.inputData.guardReleased);
					break;
				default:
					break;
				}
				if (firstSpawnReady || intervalReady) {
					// 生成条件が成立した波だけを追加し、同一所有者IDで終了管理する。
					hitBoxSystem->AddLifeTimeHitBox(owner, collData_, perent, hitBoxOwnerId_);
					++spawnedCount_;
					spawnStarted_ = true;
					nextSpawnTime_ = timer + (std::max)(collData_.hitBoxData.spawnInterval, 0.0f);
					if (collData_.hitBoxData.spawnInterval <= 0.0f) {
						// 間隔0は同一フレームに重複生成せず、単発として扱う。
						spawnedCount_ = (std::max)(1u, collData_.hitBoxData.spawnCount);
					}
				}
			}
			// 常時なら
			else if (collData_.hitBoxData.lifetimeType == HitBox::LifetimeType::kInfinite) {
				if (hitBox) {
					// 受付時間内だけ有効化し、時間外は透明なデバッグ色で無効化する
					if (timer >= collData_.hitBoxData.windowStart && timer <= collData_.hitBoxData.windowStart + collData_.hitBoxData.lifeTime) {
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
		// 次のコンボへ生成済み状態を持ち越さないようリセットする
		spawnedCount_ = 0;
		nextSpawnTime_ = 0.0f;
		spawnStarted_ = false;
		// このComboHitBoxが生成した一時判定だけを解放し、他の攻撃判定は残す。
		if (hitBoxSystem && hitBoxOwnerId_ != 0) {
			hitBoxSystem->ClearLifeTimeHitBoxes(hitBoxOwnerId_);
		}
		hitBoxOwnerId_ = 0;
		if (hitBox) {
			// 常時型ヒットボックスの当たり履歴も次回用に消す
			hitBox->Disable();
			hitBox->ClearContactRecord();
		}
	}

	void ComboHitBox::AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlobalData& combo) {
		// ヒットボックス形状に、コンボデータ側のリアクションと受付設定を合成する
		HitBox::CollData data = hitBoxData;
		// リアクションデータ
		data.reactionData = combo.hitReaction;
		data.hitBoxData = combo.hitBox;
		collData_ = data;
	};

#pragma endregion // コンボヒットボックス
};
