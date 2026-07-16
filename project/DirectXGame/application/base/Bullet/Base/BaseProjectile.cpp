#include "BaseProjectile.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/engine/Manager/Entity/EntityManager.h>
#include "DirectXGame/engine/3d/Model/RenderComponent.h"

#pragma region Base
// 初期化
void Projectile::BaseProjectile::Initialize(Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables,
	EffectSystem* effectSystem, const ProjectileSpawnInfo& spawnInfo, const ProjectileParam& param) {
	// エフェクトシステム設定
	this->effectSystem = effectSystem;
	// パラメータ保存
	param_ = param;
	// 発射情報保存
	spawnInfo_ = spawnInfo;
	// 所有者保存
	owner = spawnInfo.owner;
	// ターゲット保存
	target = spawnInfo.target;
	// 方向
	direction = spawnInfo.direction.Normalize();
	// 速度計算
	velocity = direction * param.speed;
	// 基礎初期化
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->InitializeInstancing(entity3DManager, globalVariables, param.name, param.modelName, param.textureName,
		true, true, this, Engine::ObjectInstance::TransparencyType::kNo, false);
	objectComponent_->SetInstancingSRT(spawnInfo_.scale, Math::DirectionToRotate(direction, Dire::Z), spawnInfo_.position);
	objectComponent_->GetWorldTransform().Update();
	objectComponent_->GetRigidBodyComponent()->Velocity() = velocity; // 重力加算




	// コライダー生成
	CreateCollision();


	objectComponent_->GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->GetOwner());
		if (isAlive_ == false) return;
		// ID取得
		uint32_t otherId = otherComponent->GetUniqueId();
		float nowTime = Engine::MyGame::NowTime(); // ← 時間取得関数（例）
		if (objectComponent_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		// 衝突履歴追加
		objectComponent_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);

		// コライダー処理
		CollisionProcess(otherComponent, self, other);
		// 衝突処理
		ProjectileHit();

		};



}
// 更新
void Projectile::BaseProjectile::Update() {
	if (!isAlive_) return;	// 生存していないなら更新しない
	// デルタタイム取得
	float deltaTime = objectComponent_->GetTime();
	// 生存時間更新
	lifeTime += deltaTime;
	if (lifeTime >= param_.maxLifeTime) {
		DeleteProcess();
		return;
	}
	// 移動処理
	UpdateMovement(deltaTime);
	objectComponent_->Update();
	// エフェクト処理
	UpdateEffect(deltaTime);
}
// 描画
void Projectile::BaseProjectile::Draw() {}
// 描画エフェクト
void Projectile::BaseProjectile::DrawEffect() {}
// 描画2D
void Projectile::BaseProjectile::Draw2D() {}
#pragma endregion // 基礎

#pragma region Collision

// コライダー生成
void Projectile::BaseProjectile::CreateCollision() {
	// コライダ設定(球)
	if (param_.type == Type::Laser) {
		// レーザーの場合は細長いカプセルコライダなども考えられるが、ここでは簡略化して球で設定
		auto capsule = std::make_unique<Engine::CapsuleCollider>();
		capsule->SetTag(param_.collisionTag);				// タグ設定
		capsule->SetLayer(param_.collisionLayer);			// レイヤー設定
		capsule->SetCollisionMask(param_.collisionMask);	// マスク設定
		capsule->capsule.radius = param_.radius;			// 半径を適宜設定
		capsule->Enable();								// 判定有効
		objectComponent_->GetColliderComponent()->AddCollider(std::move(capsule));	// コライダーコンポーネントにコライダ追加
	}
	else {
		auto sphere = std::make_unique<Engine::SphereCollider>();
		sphere->SetTag(param_.collisionTag);				// タグ設定
		sphere->SetLayer(param_.collisionLayer);			// レイヤー設定
		sphere->SetCollisionMask(param_.collisionMask);	// マスク設定
		sphere->radius = param_.radius;					// 半径を適宜設定
		sphere->Enable();								// 判定有効
		objectComponent_->GetColliderComponent()->AddCollider(std::move(sphere));	// コライダーコンポーネントにコライダ追加
	}
}

// コライダー処理
void Projectile::BaseProjectile::CollisionProcess(Engine::ColliderComponent* otherComponent, Engine::Collider* self, Engine::Collider* other) {
	if (param_.hitType == ProjectileHitType::Explode) {
		HitBox::CollData data_;
		data_.hitBoxData.isEneble = true;
		data_.hitBoxData.isLine = true;
		data_.hitBoxData.tag = param_.collisionTag;
		data_.hitBoxData.layer = param_.collisionLayer;
		data_.hitBoxData.colliderSize = { 1.0f,2.0f,1.0f };
		data_.name = param_.name;
		data_.reactionData.damageData.GetOne().SetDamage(param_.explosionDamage);
		data_.hitBoxData.radius = param_.explosionRadius;
		data_.hitBoxData.lifeTime = param_.explosionLifeTime;
		data_.hitBoxData.dependenceType = HitBox::ParentType::kParentIndependent;
		data_.hitBoxData.useContactRecord = true;
		data_.hitBoxData.offset = {};
		// 使用者タイプ設定
		HitBox::UseType useType = HitBox::UseType::kOther;
		if (CollisionTag::Player == self->GetTag()) {
			useType = HitBox::UseType::kPlayer;
		}
		else if (CollisionTag::Enemy == self->GetTag()) {
			useType = HitBox::UseType::kEnemy;
		}
		data_.hitBoxData.useType = useType;

		// 爆発ヒットボックス生成
		// デバッグ生成などで所有者が無い場合は、爆発判定だけをスキップして落ちないようにする。
		if (owner) {
			owner->GetHitBoxSystem()->AddLifeTimeHitBox(owner, data_, &objectComponent_->GetWorldTransform());
		}
	}

	switch (other->GetTag()) {
	case CollisionTag::Player:
	{
		// プレイヤー
		Character::BasePlayer* player = static_cast<Character::BasePlayer*>(otherComponent->GetHitReceiver());
		OnHitPlayer(player);
	}
	break;
	case CollisionTag::Enemy:
	{
		// 敵
		Character::BaseEnemy* enemy = static_cast<Character::BaseEnemy*>(otherComponent->GetHitReceiver());
		OnHitEnemy(enemy);
	}
	break;
	case CollisionTag::PlayerAttack:
	{

	}
	break;
	case CollisionTag::EnemyAttack:
	{

	}
	break;
	case CollisionTag::Wall:
	{

	}
	break;
	default:
		break;
	}
}

void Projectile::BaseProjectile::OnHitPlayer(Character::BasePlayer* player) {
	if (!player) {
		return;
	}
	if (player->GetCurrentMainState() == Character::CharacterMainState::Avoidance) {
		player->OnDodgeSuccess(); // 回避成功後コンボの受付を開く
		return; // 回避成功時はダメージと被弾ステートを入れない
	}
	if (player->IsDamageInvincible()) {
		return; // 被弾後無敵中は弾によるダメージと被弾ステートを入れない
	}
	player->AddDamage(param_.damage); // ダメージを与える
	player->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
}

void Projectile::BaseProjectile::OnHitEnemy(Character::BaseEnemy* enemy) {
	if (!enemy) {
		return;
	}
	enemy->AddDamage(param_.damage); // ダメージを与える
	enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Damage);
}

// 衝突処理
void Projectile::BaseProjectile::ProjectileHit() {
	Vector3 worldPos = objectComponent_->GetWorldPosition(); // 衝突位置取得

	switch (param_.hitType)
	{
	case ProjectileHitType::Destroy:	// 破壊
		DeleteProcess();
		break;
	case ProjectileHitType::Penetrate:	// 貫通
		// 貫通数が上限を超えたら消える
		if (pierceCount >= param_.maxPierceCount) {
			DeleteProcess();
		}
		pierceCount++;	// 貫通カウント増加
		break;
	case ProjectileHitType::Explode:	// 爆破
		DeleteProcess();
		// 爆発エフェクト出現
		effectSystem->Emit(param_.explosionEffectName, worldPos);
		break;
	case ProjectileHitType::Bounce:	// 跳ね返る
		if(param_.maxBounceCount <= bounceCount) {
			DeleteProcess();
		}
		bounceCount++;	// 跳ね返りカウント増加
		break;
	default:
		break;
	}
	// 衝突エフェクト出現
	effectSystem->Emit(param_.hitEffectName, worldPos);
}

#pragma endregion

void Projectile::BaseProjectile::UpdateMovement(float dt) {

	switch (param_.moveType)
	{
	case ProjectileMoveType::Straight: // 直線
		Straight(dt, GetWorldTransform(), direction, param_.speed);
		break;
	case ProjectileMoveType::Homing: // ホーミング
		// ターゲット未設定のホーミング弾は、直線弾として扱って安全に動かす。
		if (target) {
			Homing(dt, GetWorldTransform(), target->GetWorldPosition() + Vector3{ 0,0.5f,0 }, direction, param_.speed, param_.enableHoming, param_.homingStrength, param_.homingRange);
		}
		else {
			Straight(dt, GetWorldTransform(), direction, param_.speed);
		}
		break;
	case ProjectileMoveType::Parabola: // 放物線
		Parabola(dt, GetWorldTransform(), objectComponent_->GetRigidBodyComponent(), direction, velocity, param_.speed, param_.gravityScale);
		break;
	case ProjectileMoveType::Stay: // 維持
		Stay(dt, GetWorldTransform());
		break;
	default:
		break;
	}
}

void Projectile::BaseProjectile::UpdateEffect(float dt) {
	// 移動エフェクト出現
	effectSystem->Emit(param_.moveEffectName, objectComponent_->GetWorldPosition(), direction, direction * param_.moveEffectSpreadScale);
}

void Projectile::BaseProjectile::DeleteProcess() {
	isAlive_ = false;	// 生存フラグを下げる
	objectComponent_->Delete();	// オブジェクト削除
	objectComponent_->GetWorldTransform().scale_ = { 0,0,0 }; // スケールを0
}
