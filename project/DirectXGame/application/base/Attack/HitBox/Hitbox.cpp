#include "Hitbox.h"
#include <cmath>
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/Line/LineCommon.h"
#include <DirectXGame/engine/MyGame/MyGame.h>

namespace HitBox {

#pragma region HitBox
	// 初期化
	void HitBoxInstance::Initialize(Engine::EntityManager* entityManager, Character::BaseCharacter* character, UseType type, bool useContactRecord) {
		this->character = character;
		this->entityManager = entityManager;
		type_ = type;
		this->useContactRecord = useContactRecord;
		attackInstanceId_ = UniqueIdGenerator::Generate();
		// ワールド変換初期化
		worldTransform_.Initialize();


		// 当たり判定コンポーネント生成	
		colliderComponent_ = std::make_unique<Engine::ColliderComponent>();
		colliderComponent_->SetOwner(colliderComponent_.get());
		colliderComponent_->SetHitReceiver(this);   // 対象設定
		// ラインコモンをセット
		colliderComponent_->SetLineCommon(entityManager->Get3DLineCommon());
		// 登録（IDを取得したければ変数で受ける）
		colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

		// ヒットボックス応答クラス生成
		hitBoxFunction_ = std::make_unique<HitBoxFunction>();
		hitBoxFunction_->Initialize(colliderComponent_.get(), character, type_);
		hitBoxFunction_->SetAttackInstanceId(attackInstanceId_);

		// 当たり判定コールバック設定
		colliderComponent_->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
			// 開始
			if (!hitBoxFunction_->Begin(self, other, this->useContactRecord)) {
				return;
			}

			// 当たったコライダーによって相手に送るデータを決め転送
			for (auto& data : colliders_) {
				if (self->GetId() == data.second.colliderID) {
					hitBoxFunction_->SetData(data.second.reactionData);
					break;
				}
			}

			// 更新
			hitBoxFunction_->Update();
			};

		worldTransform_.Update();
	};

	// 更新
	void HitBoxInstance::Update(float dt) {
		// 軌道指定時は親を参照せず、生成時アンカーから現在位置を計算する。
		if (trajectoryEnabled_) {
			// 経過時間を軌道の評価時間へ加算する。
			trajectoryElapsed_ += dt;
			// 0から1の範囲へ正規化し、軌道の終点を越えないようにする。
			float normalizedTime = trajectoryDuration_ > 0.0f ? trajectoryElapsed_ / trajectoryDuration_ : 1.0f;
			if (normalizedTime < 0.0f) {
				normalizedTime = 0.0f;
			}
			if (normalizedTime > 1.0f) {
				normalizedTime = 1.0f;
			}
			// ローカル軌道を生成時のワールド基底へ変換する。
			const Vector3 localPosition = EvaluateTrajectory(normalizedTime);
			worldTransform_.parent_ = nullptr;
			worldTransform_.translate_ = ToTrajectoryWorldPosition(localPosition);
#ifdef _DEBUG
			// デバッグ時だけ軌道全体を描画し、現在の判定位置と経路を同時に確認できるようにする。
			DrawTrajectory();
#endif
		}
		// ワールド変換更新
		worldTransform_.Update();
		// 各コライダーのワールド変換更新と当たり判定コンポーネントへの反映
		for (auto& colliderData : colliders_) {
			colliderData.second.worldTransform.Update();
			colliderComponent_->UpdateByID(colliderData.second.worldTransform, colliderData.second.colliderID);
		}
	};
	// 軌道設定
	void HitBoxInstance::SetTrajectory(const GlobalHitBox& data, Engine::WorldTransform* anchor) {
		// kNoneは従来の親子付けを使うため、軌道状態を無効にする。
		trajectoryEnabled_ = data.trajectoryType != TrajectoryType::kNone;
		if (!trajectoryEnabled_) {
			return;
		}
		// 軌道設定をインスタンスへコピーし、生成後に参照元が動いても影響されないようにする。
		trajectoryType_ = data.trajectoryType;
		trajectoryElapsed_ = 0.0f;
		trajectoryDuration_ = data.trajectoryDuration > 0.0f ? data.trajectoryDuration : data.lifeTime;
		if (trajectoryDuration_ <= 0.0f) {
			trajectoryDuration_ = 1.0f;
		}
		trajectoryPoint0_ = data.trajectoryPoint0;
		trajectoryPoint1_ = data.trajectoryPoint1;
		trajectoryPoint2_ = data.trajectoryPoint2;
		trajectoryPoint3_ = data.trajectoryPoint3;
		trajectoryOrbitCenter_ = data.trajectoryOrbitCenter;
		trajectoryOrbitRadius_ = data.trajectoryOrbitRadius;
		trajectoryOrbitHeight_ = data.trajectoryOrbitHeight;
		trajectoryOrbitStartAngle_ = data.trajectoryOrbitStartAngle;
		trajectoryOrbitEndAngle_ = data.trajectoryOrbitEndAngle;
		// CreateParentで設定されたオフセットを含む現在のワールド位置を確定する。
		worldTransform_.Update();
		// 親の現在位置と基底軸だけを保存し、以後の親移動から切り離す。
		trajectoryAnchorPosition_ = worldTransform_.GetWorldPosition();
		if (anchor != nullptr) {
			trajectoryRight_ = { anchor->worldMat_.m[0][0], anchor->worldMat_.m[0][1], anchor->worldMat_.m[0][2] };
			trajectoryUp_ = { anchor->worldMat_.m[1][0], anchor->worldMat_.m[1][1], anchor->worldMat_.m[1][2] };
			trajectoryForward_ = { anchor->worldMat_.m[2][0], anchor->worldMat_.m[2][1], anchor->worldMat_.m[2][2] };
		}
		// 軌道使用時は親ポインターを解除して、キャラクターや武器ボーンから独立させる。
		worldTransform_.parent_ = nullptr;
		worldTransform_.translate_ = trajectoryAnchorPosition_;
		worldTransform_.Update();
	}

	// 軌道座標変換
	Vector3 HitBoxInstance::ToTrajectoryWorldPosition(const Vector3& localPosition) const {
		// 生成時に保存した右・上・前の基底軸でローカル座標をワールド座標へ変換する。
		return {
			trajectoryAnchorPosition_.x + trajectoryRight_.x * localPosition.x + trajectoryUp_.x * localPosition.y + trajectoryForward_.x * localPosition.z,
			trajectoryAnchorPosition_.y + trajectoryRight_.y * localPosition.x + trajectoryUp_.y * localPosition.y + trajectoryForward_.y * localPosition.z,
			trajectoryAnchorPosition_.z + trajectoryRight_.z * localPosition.x + trajectoryUp_.z * localPosition.y + trajectoryForward_.z * localPosition.z
		};
	}

	// 選択中コンボ用の軌道デバッグ描画
	void HitBoxInstance::DrawTrajectoryDebug(const GlobalHitBox& data, Engine::WorldTransform* anchor, Engine::LineCommon* lineCommon) {
#ifdef _DEBUG
		// 軌道未使用または描画先がない場合は何もしない。
		if (data.trajectoryType == TrajectoryType::kNone || lineCommon == nullptr) {
			return;
		}
		// 実行時と同じ軌道評価処理を使うため、一時インスタンスへ設定をコピーする。
		HitBoxInstance preview;
		preview.SetTrajectory(data, anchor);
		preview.DrawTrajectory(lineCommon->GetDebugLineMeshData());
#else
		(void)data;
		(void)anchor;
		(void)lineCommon;
#endif
	}

	// 軌道デバッグ描画
	void HitBoxInstance::DrawTrajectory() const {
		// EntityManagerまたはライン共通クラスがない場合は描画を省略する。
		if (entityManager == nullptr || entityManager->Get3DLineCommon() == nullptr) {
			return;
		}
		DrawTrajectory(entityManager->Get3DLineCommon()->GetDebugLineMeshData());
	}

	// 軌道折れ線描画
	void HitBoxInstance::DrawTrajectory(Engine::LineMeshData& lineMeshData) const {
		// 軌道を32分割して折れ線として描画する。
		constexpr int kTrajectorySegmentCount = 32;
		const Vector4 trajectoryColor = { 1.0f, 0.8f, 0.1f, 1.0f };
		Vector3 previousPoint = ToTrajectoryWorldPosition(EvaluateTrajectory(0.0f));
		for (int segment = 1; segment <= kTrajectorySegmentCount; ++segment) {
			const float normalizedTime = static_cast<float>(segment) / static_cast<float>(kTrajectorySegmentCount);
			const Vector3 currentPoint = ToTrajectoryWorldPosition(EvaluateTrajectory(normalizedTime));
			lineMeshData.AddLine(previousPoint, currentPoint, trajectoryColor);
			previousPoint = currentPoint;
		}
	}
	// 軌道評価
	Vector3 HitBoxInstance::EvaluateTrajectory(float normalizedTime) const {
		// 軌道の種類に応じたローカル座標を計算する。
		if (trajectoryType_ == TrajectoryType::kBezier) {
			const float inverseTime = 1.0f - normalizedTime;
			const float a = inverseTime * inverseTime * inverseTime;
			const float b = 3.0f * inverseTime * inverseTime * normalizedTime;
			const float c = 3.0f * inverseTime * normalizedTime * normalizedTime;
			const float d = normalizedTime * normalizedTime * normalizedTime;
			return {
				trajectoryPoint0_.x * a + trajectoryPoint1_.x * b + trajectoryPoint2_.x * c + trajectoryPoint3_.x * d,
				trajectoryPoint0_.y * a + trajectoryPoint1_.y * b + trajectoryPoint2_.y * c + trajectoryPoint3_.y * d,
				trajectoryPoint0_.z * a + trajectoryPoint1_.z * b + trajectoryPoint2_.z * c + trajectoryPoint3_.z * d
			};
		}
		if (trajectoryType_ == TrajectoryType::kCatmullRom) {
			const float t2 = normalizedTime * normalizedTime;
			const float t3 = t2 * normalizedTime;
			return {
				0.5f * ((2.0f * trajectoryPoint1_.x) + (-trajectoryPoint0_.x + trajectoryPoint2_.x) * normalizedTime + (2.0f * trajectoryPoint0_.x - 5.0f * trajectoryPoint1_.x + 4.0f * trajectoryPoint2_.x - trajectoryPoint3_.x) * t2 + (-trajectoryPoint0_.x + 3.0f * trajectoryPoint1_.x - 3.0f * trajectoryPoint2_.x + trajectoryPoint3_.x) * t3),
				0.5f * ((2.0f * trajectoryPoint1_.y) + (-trajectoryPoint0_.y + trajectoryPoint2_.y) * normalizedTime + (2.0f * trajectoryPoint0_.y - 5.0f * trajectoryPoint1_.y + 4.0f * trajectoryPoint2_.y - trajectoryPoint3_.y) * t2 + (-trajectoryPoint0_.y + 3.0f * trajectoryPoint1_.y - 3.0f * trajectoryPoint2_.y + trajectoryPoint3_.y) * t3),
				0.5f * ((2.0f * trajectoryPoint1_.z) + (-trajectoryPoint0_.z + trajectoryPoint2_.z) * normalizedTime + (2.0f * trajectoryPoint0_.z - 5.0f * trajectoryPoint1_.z + 4.0f * trajectoryPoint2_.z - trajectoryPoint3_.z) * t2 + (-trajectoryPoint0_.z + 3.0f * trajectoryPoint1_.z - 3.0f * trajectoryPoint2_.z + trajectoryPoint3_.z) * t3)
			};
		}
		if (trajectoryType_ == TrajectoryType::kOrbit) {
			const float angle = trajectoryOrbitStartAngle_ + (trajectoryOrbitEndAngle_ - trajectoryOrbitStartAngle_) * normalizedTime;
			return {
				trajectoryOrbitCenter_.x + std::cos(angle) * trajectoryOrbitRadius_,
				trajectoryOrbitCenter_.y + trajectoryOrbitHeight_,
				trajectoryOrbitCenter_.z + std::sin(angle) * trajectoryOrbitRadius_
			};
		}
		// 未知の値は安全のため原点に留める。
		return {};
	}
#pragma endregion

#pragma region MyRegion
	// コライダー追加
	void HitBoxInstance::AddCollider(std::unique_ptr<Engine::Collider> collider, const Vector3& offset, const HitReactionData& reaction) {
		// 当たり判定コンポーネントにコライダー追加
		ColliderData data;
		data.colliderID = colliderComponent_->AddCollider(std::move(collider));
		data.collider = colliderComponent_->FindColliderById(data.colliderID);
		data.collider->SetIsDebugLine(true);

		worldTransform_.Update();

		// ワールド変換設定
		data.worldTransform.Initialize();
		data.worldTransform.translate_ = offset;
		data.worldTransform.parent_ = &worldTransform_;
		data.worldTransform.Update();
		// リアクションデータ
		data.reactionData = reaction;

		// コライダー情報を配列に追加
		colliders_[data.colliderID] = data;
		colliderCount++; // コライダー数増加
	};

	void HitBoxInstance::SetRecordPerCollider(bool enabled) {
		if (hitBoxFunction_) {
			hitBoxFunction_->SetRecordPerCollider(enabled);
		}
	}

	// 有効化
	void HitBoxInstance::Enable(Vector4 color) {
		for (auto& colliderData : colliders_) {
			colliderData.second.collider->Enable();
			colliderData.second.collider->SetLineColor(color);
		}
	};
	// 無効化
	void HitBoxInstance::Disable(Vector4 color) {
		for (auto& colliderData : colliders_) {
			colliderData.second.collider->Disable();
			colliderData.second.collider->SetLineColor(color);
		}
	};

#pragma endregion
}
