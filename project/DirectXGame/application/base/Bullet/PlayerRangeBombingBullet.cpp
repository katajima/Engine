#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"

PlayerRangeBombingBullet::~PlayerRangeBombingBullet()
{
	// エミッター削除
	effectComponent_->GetGpuParticleManager()->ClearEmitter("MoveEmitte_" + std::to_string(countIndex_));
	effectComponent_->RemoveEmitterAll(); // エミッター全削除
	effectComponent_->RemovePrimitiveAll();	// プリミティブ形状削除
	effectComponent_->RemoveTrailEffectAll();	// トレイルエフェクト削除
}
//
void PlayerRangeBombingBullet::Initialize(Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	// 基盤の初期化
	BaseInitialize(entityManager, globalVariables, position,camera, "playerbullet", "player_bullet.obj");
	//// オブジェクト設定
	object_->Update();	// オブジェクト更新
	object_->InitColliderComponent();	// コライダコンポーネント初期化
	

	trailTransform_.Initialize();
	trailTransform_.parent_ = &object_->GetWorldTransform();
	trailTransform_.Update();

	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<Engine::EffectComponent>();
	effectComponent_->Init(entityManager,globalVariables);
	// トレイル描画のビュープロジェクション計算に使うカメラを設定する。
	effectComponent_->SetCamera(camera);

	// コライダ設定
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->SetTag(CollisionTag::PlayerAttack);			// タグ設定
	sphere->SetLayer(CollisionLayer::PlayerAttack);		// レイヤー設定
	sphere->SetCollisionMask(static_cast<uint32_t>(CollisionLayer::Enemy));	// マスク設定
	sphere->radius = provisionalData_.collRadius; // 半径を適宜設定
	sphere->Enable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));	// コライダコンポーネントに追加	



	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->GetOwner());
		if (!otherComponent || other->GetTag() != CollisionTag::Enemy) return;

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
		// 敵ステート変更
		enemy->GetCharacterStateMachine()->ChangeState(Character::CharacterMainState::Move);
		// 必殺ミサイルのダメージとして記録し、撃破時にSPを出さない判定へつなげる。
		enemy->ApplyAttackDamage(parameter_.damage, AttackAttribute::Special);

		this->owner->GetAttackController()->GetHitCounter().Hit();
		
		// エフェクト出現
		enemy->GetHitMotionSystem()->EmitHitEffect();
		};


	// Y軸周り角度(θy)
	object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

	// ダメージ量
	parameter_.damage = provisionalData_.damage;
	// 生存フラグ
	isAlive_ = true;
	// 行動フェーズ
	phase_ = 0;


	// 初期地点記録
	str = object_->GetWorldTransform().translate_;

	// ランダムな空中位置を設定
	randPosSky = str + Vector3{ -provisionalData_.skyX,0,0 };

	// 最大Y座標
	max_y = provisionalData_.skyY;

	// ランダム位置のY座標を設定
	randPosSky.y = max_y;


	
	//// 各演出設定
	//// ミサイル移動中の煙パーティクルエミッター
	countIndex_ = UniqueIdGenerator::Generate();

	// 弾の移動軌跡を細いリボン状に残すため、専用設定でトレイルを生成する。
	Engine::TrailSettings trailSettings{};
	trailSettings.minEmitDistance = provisionalData_.trailMinEmitDistance;
	trailSettings.maxSegmentCount = provisionalData_.trailMaxSegmentCount;
	effectComponent_->AddTrailEffect(trailName_ + std::to_string(countIndex_), "resources/Texture/Image.dds",
		provisionalData_.trailLifeTime, trailTransform_, trailSettings, provisionalData_.trailColor,
		{ 0.0f, provisionalData_.trailWidth, 0.0f }, { 0.0f, -provisionalData_.trailWidth, 0.0f });
	effectComponent_->GetTrailEffect(trailName_ + std::to_string(countIndex_))->SetIsEmit(true);

	// シリンダーパラメーター設定
	Engine::ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = provisionalData_.cylinderHeight_;
	cylinderParam.innerRadius = provisionalData_.innerRadius;
	cylinderParam.outerRadius = provisionalData_.outerRadius;
	cylinderParam.isCover = true;
	cylinderParam.segments = provisionalData_.segments;


	// シリンダー生成
	std::unique_ptr<Engine::CylinderPrimitive> cylinder2 = std::make_unique<Engine::CylinderPrimitive>();
	cylinder2->Data() = cylinderParam;
	cylinder2->Initialize(entityManager->GetPrimitiveCommon(), "resources/Texture/Image.dds");


	
	// 当たった時のオブジェクト生成
	hitObject2_ = entityManager->CreatePrimitiveObject3D<Engine::CylinderPrimitive>("cylinder", "resources/Texture/Image.dds", camera);
	hitObject2_->SetPrimitive(std::move(cylinder2));
	hitObject2_->GetPrimitive()->SetPsoType(Engine::BasePrimitive::PsoType::kRingClamp);
	hitObject2_->SetIsDraw(false);
	hitObject2_->GetPrimitive()->GetMaterial()->GetMaterialInstance().color = provisionalData_.hit2Color;

	// 敵の位置捕捉
	enemyPos_.x = targetRange_.position.x + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.z = targetRange_.position.z + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
}

// 更新
void PlayerRangeBombingBullet::Update() {
	// 弾のワールド座標
	Vector3 worldPos = object_->GetWorldPosition();
	Vector3& translate = object_->GetWorldTransform().translate_;
	Vector3 norm{};
	// 当たったら死ぬ
	if (Hit) {
		effect->Emit("missileHit", worldPos);
		isAlive_ = false;
	}
	
	// 生きているなら
	if (isAlive_) {
		
		switch (phase_)	// フェーズ(移動)
		{
		case 0:
		{
			if (phase0Timer_ > 0.0f) {
			}
			else {
				// 煙出す
				effect->Emit("stratSmoke01", worldPos + Vector3{ 10.0f,-5.0f,0.0f });
			}
			// 時間更新
			phase0Timer_ += GetTimer();

			float t = phase0Timer_ / phase0EndTime_;

			// 位置を線形補間
			translate = Lerp(str, randPosSky, t);

			// 移動方向の設定
			norm = randPosSky - str;
			velocity_ = norm * GetTimer();

			// tが1に達したら
			if (t >= 1) {
				phase_++;	// フェーズ移行
			}
			break;
		}
		case 1:	// フェーズ(落下待機)

			
			// カウント更新
			phase1Timer_ += GetTimer();

			// カウントが最大値を達したら
			if (phase1Timer_ >= phase1EndTime_)
			{
				// 敵の位置に向かって落ちる
				Vector3 pos = enemyPos_ - worldPos;
				float posLength = Length(pos);
				// 方向指定
				Vector3 pos2 = pos;
				velocity_ = pos2.Normalize() * provisionalData_.speedPhase1;
				velocity_.y = 0; // 垂直落下

				// オブジェクトの位置更新
				translate += velocity_ * GetTimer();

				// 敵の範囲内に入ったら
				if (provisionalData_.distanceToEnemy >= DistanceXZ(worldPos, enemyPos_)) {
					phase_++; // フェーズ移行
					posGround = enemyPos_; // 落ちる位置決定

					// 
					targetPos = posGround - worldPos;
					// ヒットオブジェクト描画
					hitObject2_->SetIsDraw(true);
				}
			}
			break;
		case 2:	// フェーズ(落下)
			// カウント更新
			phase2Timer_ += GetTimer();
			
			// リングエフェクトの位置を設定
			effect->Emit("ringEmit", worldPos);

			// カウントが最大値を達したら
			if (phase2Timer_ >= phase2EndTime_)
			{
				velocity_ = targetPos * provisionalData_.speedPhase2;	// 速度設定
				translate += velocity_ * GetTimer();	// 位置更新
			}

			// 0.0f以下になったら
			if (-0.0f >= object_->GetWorldPosition().y) {
				// 位置を固定
				translate.y = 0.5f;
				// 死亡
				isAlive_ = false;
				phase_ = 0;	// フェーズ初期化
				
				isEffectPlay_ = true;	// エフェクト再生

				object_->GetWorldTransform().Update(); // オブジェクト位置更新
				
				// ヒットオブジェクト描画しない
				hitObject2_->SetIsDraw(false);


				// それぞれのパーティクル出現
				effect->Emit("missileHitCylinder", worldPos);
				effect->Emit("missileHit", worldPos + Vector3{ 0,provisionalData_.hitCylinderY,0 });
				effect->Emit("smokePlaneExpSmoke", worldPos);
				effect->Emit("animatedCube", worldPos);
				effect->Emit("expPlane01", worldPos + Vector3{ 0,provisionalData_.expPlaneY,0 });
				effect->Emit("expSpark", worldPos);
			}
			break;
		}
		
		

		// Y軸周り角度(θy)
		object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Length(Vector3(velocity_.x, 0, velocity_.z));
		// X軸周り角度(θx)
		object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

		// ヒットオブジェクト位置と回転設定
		hitObject2_->GetWorldTransform().translate_ = posGround + Vector3{ 0,provisionalData_.hitObjectY,0 };
		hitObject2_->GetWorldTransform().rotate_.x = Math::DegreesToRadians(provisionalData_.hitObjectRotateX);
	}

	// トレイルが最新の弾位置を参照できるよう、親のワールド行列を先に更新する。
	object_->GetWorldTransform().Update();
	// トレイル発生
	trailTransform_.Update();
	// エフェクト再生
	if (isEffectPlay_) {
		time_ += GetTimer();	// 時間更新
		object_->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);	// コライダ有効
		object_->SetIsDraw(false);	// オブジェクト描画しない

		// 時間に達したら
		if (time_ >= 0.5f) {
			object_->IsDelete();	// オブジェクト削除
			hitObject2_->IsDelete();// ヒットオブジェクト削除
			isEffectPlay_ = false;	// エフェクト終了
			object_->GetColliderComponent()->ClearColliders();	// コライダクリア
		}
	}
	effectComponent_->Update();	// エフェクトコンポーネント更新
}

// 描画
void PlayerRangeBombingBullet::Draw()
{
	effectComponent_->Draw();	// エフェクトコンポーネント描画
}

// エフェクト描画
void PlayerRangeBombingBullet::DrawEffect()
{
	effectComponent_->DrawEffect();
}

// 2D描画
void PlayerRangeBombingBullet::Draw2D()
{
}


