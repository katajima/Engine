#include "PlayerBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"

// 初期化
void PlayerBullet::Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
	// 基盤の初期化
	BaseInitialize(entity3DManager, entity2DManager, globalVariables, position, camera, "playerStanBullet", "AnimatedCube.gltf");

	// オブジェクト生成
	object_->GetWorldTransform().scale_ = provisionalData_.objectSize; // スケール設定
	object_->UseTrailEffect("resources/Texture/Image.png", provisionalData_.trailLifeTime, Color::WHITE(), {0,provisionalData_.trailWidth,0}, {0,-provisionalData_.trailWidth,0}); // トレイル設定
	object_->Update();	// オブジェクト更新
	object_->isEmitTrailEffect = false;	// トレイルの出現しないように
	object_->InitColliderComponent();	// コライダコンポーネント初期化
	object_->SetIsDraw(true);			// 描画する


	// コライダ設定(球)
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;		// タグ設定
	sphere->layer = CollisionLayer::PlayerAttack;	// レイヤー設定
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));// マスク設定
	sphere->radius = provisionalData_.collRadius; // 半径を適宜設定
	sphere->Enable();	// 判定有効
	object_->GetColliderComponent()->AddCollider(std::move(sphere));	// コライダーコンポーネントにコライダ追加

	// ダメージ設定
	parameter_.damage = info_.damage;



	// 移動方向指定
	velocity_ = Normalize(info_.targetPos - object_->GetWorldTransform().GetWorldPosition()) * info_.speed;


	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;
		if (isAlive_ == false) return;
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(otherComponent->GetHitReceiver());
		// ID取得
		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = Engine::MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		// 衝突履歴追加
		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		// 敵にダメージ
		enemy->AddDamage(parameter_.damage);

		if (enemy->GetCharacterParameterComponent()->GetHP() > 0) {
			enemy->Emit();	// エフェクト出現
		}
		// 弾が当たったら消えるか
		if (info_.type == ProjectileType::NORMAL) {
			hitDelete_ = true;	// 当たったら消える
		}
		};

	timer_ = 0.0f;		// 時間初期化
	isAlive_ = true;	// 生存フラグ設定

};

// 毎フレーム更新
void PlayerBullet::Update() {

	// 時間が初期値出なければ
	if (timer_ != 0.0f) {
		object_->isEmitTrailEffect = true;
	}
	// 時間更新
	timer_ += GetTimer();

	// y軸が-3.0f以下になったら
	if (object_->GetWorldTransform().translate_.y <= groundHeight_) {
		velocity_ = 0.0f;	// 速度0に
		object_->GetWorldTransform().translate_.y = groundHeight_;	// 位置固定
		hitDelete_ = true;	// 削除
	}
	else {
		object_->GetWorldTransform().translate_ += velocity_ * GetTimer();	// 位置更新
	}

	// 削除時間に達するか当たったら
	if (deleTimer_ <= timer_ || hitDelete_) {
		Final(); // 最終処理
	}
};

// 描画
void PlayerBullet::Draw() {
};
//描画エフェクト
void PlayerBullet::DrawP() {
};
//描画2d
void PlayerBullet::Draw2D() {

}
void PlayerBullet::Final()
{
	isEffectPlay_ = true;		// エフェクト再生
	isAlive_ = false;			// 死亡に
	object_->SetIsDraw(false);	// 描画しない
	deleTimer_ += GetTimer();	// 削除タイマー
	if (deleTimer_ >= 0.1f) {	// 削除タイマーが達したら
		isEffectPlay_ = false;	// エフェクト終了
		object_->IsDelete();	// オブジェクト削除
		object_->GetColliderComponent()->ClearColliders(); // コライダクリア
	}
}
;