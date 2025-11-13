#include "PlayerStanBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"


// 初期化
void PlayerStanBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) {
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目
		
	// オブジェクト生成
	object_ = entity3DManager->CreateObject3D("playerStanBullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("AnimatedCube.gltf");			// モデル指定
	object_->GetWorldTransform().scale_ = { 2,2,2 };// スケール設定
	object_->Update();								// オブジェクト更新
	object_->InitColliderComponent();				// コライダコンポーネント初期化
	object_->SetIsDraw(true);						// 描画する
	

	// コライダ追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;		// タグ設定
	sphere->layer = CollisionLayer::PlayerAttack;	// レイヤー設定
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));// マスク設定
	sphere->radius = 20.0f; // 半径を適宜設定	
	sphere->Disable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));// コライダコンポーネントにコライダ追加

	// ダメージ設定
	parameter_.damege = 0;
	
	
	// 移動方向設定
	velocity_ = player_->GetObjectComponent()->GetObject3D()->ObjectDirection(); // 向いている方向に
	velocity_ *= 30.0f;
	velocity_.y = 100.0f;
	
	
	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;

		// 敵
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());
		// ID取得
		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		// 衝突履歴追加
		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		// ダメージ
		enemy->AddDamage(parameter_.damege);

		// ステート変更(怯み状態に)
		enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Fainting);
		// エフェクト出現
		enemy->Emit();
		};

	// 時間初期化
	timer_ = 0.0f;
	deleTimer_ = 0.0f;
	isAlive_ = true; // 生存

};

// 毎フレーム更新
void PlayerStanBullet::Update() {
	SphereCollider* coll = object_->GetColliderComponent()->FindColliderById<SphereCollider>(1);
		
	// 時間更新
	timer_ += GetTimer();


	
	// y軸が-3.0f以上なら
	if (object_->GetWorldPosition().y >= -3.0f) {
		velocity_.y += -9.8f;	// 重力
		object_->GetWorldTransform().translate_ += velocity_ * GetTimer(); // 位置更新
	}

	// 高さが-3.0以下になったら
	if (object_->GetWorldTransform().translate_.y <= -3.0f) {
		velocity_ = 0.0f;	// 移動しない
		object_->GetWorldTransform().translate_.y = -3.0f;	// 位置固定
	}
	

	
	// 爆破時間になったら
	if (explodingtimer_ <= timer_) {
		coll->enabled = true;		// コライダ有効
		parameter_.damege = 1.0f;	// ダメージ設定
		isEffectPlay_ = true;		// エフェクト再生
		isAlive_ = false;			// 死亡
		object_->SetIsDraw(false);	// オブジェクト描画しない
		deleTimer_ += GetTimer();	// 削除タイマー更新
		if (deleTimer_ >= 0.1f) {	// 削除タイマーに達したら
			isEffectPlay_ = false;	// エフェクト終了
			object_->IsDelete();	// オブジェクト削除
			object_->GetColliderComponent()->ClearColliders();	// コライダクリア
		}
	}
};

// 描画
void PlayerStanBullet::Draw() {

};
//
void PlayerStanBullet::DrawP() {

};
//
void PlayerStanBullet::Draw2D() {

};