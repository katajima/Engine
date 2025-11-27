#include "playerWeapon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

void PlayerWeapon::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	input_ = input;						// インプット
	entity3DManager_ = entity3DManager; // エンティティ3d
	entity2DManager_ = entity2DManager; // エンティティ2d
	

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entity3DManager_, globalVariables_, "PlayerWeapon", "Sword.obj", true, false, this);
	objectComponent_->SetSRT(provisionalData_.size, {}, position);	// SRT設定

	// トレイルエフェクト設定
	GetObject3D()->UseTrailEffect("resources/Texture/Image.png", provisionalData_.trailLifeTime, provisionalData_.color,
		GetObject3D()->GetModel()->modelData.mesh[0]->GetMin(),
		GetObject3D()->GetModel()->modelData.mesh[0]->GetMax());
	
	// コライダーの更新は手動で行うため、Object3d内での更新無効化
	GetObject3D()->SetIsUpdateColliderComponent(false); 
	// 描画する
	GetObject3D()->SetIsDraw(true);
	// スケール3
	GetObject3D()->GetWorldTransform().scale_ = provisionalData_.scale_;


	// コライダ根本(OBB)
	auto obbCollider_ = std::make_unique<OBBCollider>();
	obbCollider_->obb.size = provisionalData_.obbColliderSize;			// サイズ設定
	obbCollider_->tag = CollisionTag::PlayerAttack;			// タグ設定
	obbCollider_->layer = CollisionLayer::PlayerAttack;		// レイヤー設定
	obbCollider_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));	// マスク設定
	obbCollider_->Enable();																// 有効
	obbCollider_->isDebugLine = true;													// デバッグライン描画
	weaponColliderId_ = GetObject3D()->GetColliderComponent()->GetNextId();				// ID設定
	GetObject3D()->GetColliderComponent()->AddCollider(std::move(obbCollider_));		// コライダ追加

	// コライダ先端(OBB)
	auto obbCollider2_ = std::make_unique<OBBCollider>();
	obbCollider2_->obb.size = provisionalData_.obbCollider2Size;			// サイズ設定
	obbCollider2_->tag = CollisionTag::PlayerAttack;		// タグ設定
	obbCollider2_->layer = CollisionLayer::PlayerAttack;	// レイヤー設定
	obbCollider2_->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));	// マスク設定
	obbCollider2_->Enable();															// 有効
	obbCollider2_->isDebugLine = true;													// デバッグライン描画
	weaponColliderId2_ = GetObject3D()->GetColliderComponent()->GetNextId();			// ID設定
	GetObject3D()->GetColliderComponent()->AddCollider(std::move(obbCollider2_));		// コライダ追加

	
	// 衝突応答
	GetObject3D()->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		if (!other || other->tag != CollisionTag::Enemy) return;

		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		if (!enemy) return;

		const uint32_t otherId = otherComponent->GetUniqueId();	// ID取得
		const float nowTime = MyGame::NowTime();				// 現在時間

		NormalPlayer* player = static_cast<NormalPlayer*>(character);

		if (GetObject3D()->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		// 履歴追加
		GetObject3D()->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		// ヒット
		//enemy->GetResponseSystem()->GetHitMotionComponent()->SetIsHit(true);




		// ノックバック方向
		comboData_.knockbackData.SetNormal(player->GetMoveComponent()->GetDirection());
		//enemy->GetResponseSystem()->GetHitMotionComponent()->SetIsKnockback(true);		// ノックバックするか

		// ダメージ設定
		float damage = comboData_.damage.GetDamage();
		if (self->id == weaponColliderId_) {
			//enemy->GetResponseSystem()->GetHitMotionComponent()->SethitStopTime(1.1f);	// ヒットストップ時間設定
			//enemy->GetResponseSystem()->GetHitMotionComponent()->SetKnockbackTime(0.3f);	// ノックバック時間設定
			damage *= 2;	// ダメージ設定
		}
		else if (self->id == weaponColliderId2_) {
			//enemy->GetResponseSystem()->GetHitMotionComponent()->SethitStopTime(1.1f);	// ヒットストップ時間設定
			//enemy->GetResponseSystem()->GetHitMotionComponent()->SetKnockbackTime(0.3f);	// ノックバック時間設定
			damage *= 1;	// ダメージ設定
		}
		enemy->GetResponseSystem()->GetHitMotionSystem()->SetKnockbackData(comboData_.knockbackData);	 // ノックバックデータ設定
		enemy->AddDamage(DamageCalculator::ComputeDamageWeapon(*character->GetCombatStatComponent(), *enemy->GetCombatStatComponent(), damage));



		enemy->Emit();	//	エフェクト出現
		enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move); // 敵ステート設定
		player->AddHit();		// ヒット回数加算
		player->AddSpGauge(1);	// スペシャルゲージ増化
		player->SetHitTime();	// ヒットリセット時間をリセット
		};

	// コンボデータ設定
	ComboData data;
	data.damage.SetDamage(provisionalData_.damage1);
	data.knockbackData.SetPower(provisionalData_.knockbackPower1, provisionalData_.knockbackPowerY1);
	

	attack1 = std::make_shared<ComboNodeState>("Attack1", data);
	data.damage.SetDamage(provisionalData_.damage2);
	attack2 = std::make_shared<ComboNodeState>("Attack2", data);
	data.damage.SetDamage(provisionalData_.damage3);
	data.knockbackData.SetPower(provisionalData_.knockbackPower2, provisionalData_.knockbackPowerY2);
	data.knockbackData.GetData().isVerticalBoost_ = true;
	attack3 = std::make_shared<ComboNodeState>("Attack3", data);
	
	AddComboNode("Attack1", attack1);	// コンボ追加
	AddComboNode("Attack2", attack2);	// コンボ追加
	AddComboNode("Attack3", attack3);	// コンボ追加

	ConnectCombo("Attack1", AttackInput::Light, "Attack2"); // コンボ連結
	ConnectCombo("Attack1", AttackInput::Heavy, "Attack3"); // コンボ連結
	ConnectCombo("Attack2", AttackInput::Light, "Attack3"); // コンボ連結

	
	// コライダ位置初期化
	colliderWorld_.Initialize();
	colliderWorld_.parent_ = &GetObject3D()->GetWorldTransform();
	colliderWorld_.translate_ = provisionalData_.collider1Pos; // 武器の位置調整
	
	// コライダ位置初期化
	colliderWorld2_.Initialize();
	colliderWorld2_.parent_ = &colliderWorld_;
	colliderWorld2_.translate_ = provisionalData_.collider2Pos; // 武器の位置調整

}

void PlayerWeapon::Update()
{
	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
	
	// コライダのワールドトランスフォーム更新
	colliderWorld_.Update();
	colliderWorld2_.Update();

	// コライダコンポーネントにコライダの位置を送る
	GetObject3D()->GetColliderComponent()->UpdateByID(colliderWorld_, weaponColliderId_);
	GetObject3D()->GetColliderComponent()->UpdateByID(colliderWorld2_, weaponColliderId2_);
}

void PlayerWeapon::Draw2D()
{
}

void PlayerWeapon::DrawEffect()
{
}

