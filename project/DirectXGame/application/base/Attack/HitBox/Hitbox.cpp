#include "Hitbox.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include <DirectXGame/engine/MyGame/MyGame.h>


#pragma region HitBox

// 初期化
void HitBox::Initialize(Engine::Entity3DManager* entity3dManager, BaseCharacter* character, HitBoxUseType type) {
	character_ = character;
	entity3dManager_ = entity3dManager;
	type_ = type;

	// ワールド変換初期化
	worldTransform_.Initialize();


	// 当たり判定コンポーネント生成	
	colliderComponent_ = std::make_unique<Engine::ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	colliderComponent_->SetHitReceiver(this);   // 対象設定
	// ラインコモンをセット
	colliderComponent_->SetLineCommon(entity3dManager->Get3DLineCommon());
	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

	// ヒットボックス応答クラス生成
	hitBoxFunction_ = std::make_unique<HitBoxFunction>();
	hitBoxFunction_->Initialize(colliderComponent_.get(), character, type);

	// 当たり判定コールバック設定
	colliderComponent_->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		// 開始
		if (!hitBoxFunction_->Begin(self, other)) {
			return;
		};
		
		// 当たったコライダーによって相手に送るデータを決め転送
		for (auto& data : colliders_) {
			if (self->id == data.second.colliderID) {
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
void HitBox::Update(float dt) {
	// ワールド変換更新
	worldTransform_.Update();
	// 当たり判定コンポーネント更新

	// 各コライダーのワールド変換更新と当たり判定コンポーネントへの反映
	for (auto& colliderData : colliders_) {
		colliderData.second.worldTransform.Update();
		colliderComponent_->UpdateByID(colliderData.second.worldTransform, colliderData.second.colliderID);
	}
};
#pragma endregion

#pragma region MyRegion
// コライダー追加
void HitBox::AddCollider(std::unique_ptr<Engine::Collider> collider,const Vector3& offset, const AttackReactionData& reaction) {
	// 当たり判定コンポーネントにコライダー追加
	ColliderData data;
	data.colliderID = colliderComponent_->AddCollider(std::move(collider));
	data.collider = colliderComponent_->FindColliderById(data.colliderID);


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

// 有効化
void HitBox::Enable() {
	for (auto& colliderData : colliders_) {
		colliderData.second.collider->Enable();
	}
};
// 無効化
void HitBox::Disable() {
	for (auto& colliderData : colliders_) {
		colliderData.second.collider->Disable();
	}
};

#pragma endregion

