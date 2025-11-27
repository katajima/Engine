#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

PlayerRangeBombingBullet::~PlayerRangeBombingBullet()
{
	// エミッター削除
	effectComponent_->GetGpuParticleManager()->ClearEmitter("MoveEmitte_" + std::to_string(countIndex_));
	effectComponent_->RemoveEmitterAll(); // エミッター全削除
	effectComponent_->RemovePrimitiveAll();	// プリミティブ形状削除
	effectComponent_->RemoveTrailEffectAll();	// トレイルエフェクト削除
}
//
void PlayerRangeBombingBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	globalVariables_ = globalVariables;	// 保存項目
	entity3DManager_ = entity3DManager; // エンティティ3d
	//// オブジェクト設定
	object_ = entity3DManager->CreateObject3D("playerbullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("player_bullet.obj");	// モデル設定
	object_->UseTrailEffect("resources/Texture/Image.png", provisionalData_.trailLifeTime, Color::WHITE(), {0,provisionalData_.trailWidth,0}, {0,-provisionalData_.trailWidth,0}); // トレイル設定
	object_->isEmitTrailEffect = false; // トレイルを出現させない
	object_->Update();	// オブジェクト更新
	object_->InitColliderComponent();	// コライダコンポーネント初期化
	

	// エフェクトコンポーネント初期化
	effectComponent_ = std::make_unique<EffectComponent>();	
	effectComponent_->Init(entity3DManager_,globalVariables_);	


	// コライダ設定
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;			// タグ設定
	sphere->layer = CollisionLayer::PlayerAttack;		// レイヤー設定
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));	// マスク設定
	sphere->radius = provisionalData_.collRadius; // 半径を適宜設定
	sphere->Enable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));	// コライダコンポーネントに追加	



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
		// 敵ステート変更
		enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		// ダメージ
		enemy->AddDamage(parameter_.damege);
		// エフェクト出現
		enemy->Emit();
		};


	// Y軸周り角度(θy)
	object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

	// ダメージ量
	parameter_.damege = provisionalData_.damage;
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

	// 移動中の煙エフェクト初期化
	InitMoveSmoke();

	// シリンダーパラメーター設定
	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = cilnderHeight_;
	cylinderParam.innerRadius = provisionalData_.innerRadius;
	cylinderParam.outerRadius = provisionalData_.outerRadius;
	cylinderParam.isCover = false;
	cylinderParam.segments = provisionalData_.segments;

	// シリンダー生成
	std::unique_ptr<CylinderPrimitive> cylinder2 = std::make_unique<CylinderPrimitive>();
	cylinder2->Data() = cylinderParam;
	cylinder2->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/effect/gradationLine.png");


	cylinderParam.height = provisionalData_.height;
	cylinderParam.innerRadius = provisionalData_.innerRadiusHit2;
	cylinderParam.isCover = true;

	// 当たった時のオブジェクト生成
	hitObject2_ = entity3DManager->CreatePrimitiveObject3D<CylinderPrimitive>("cylinder", "resources/Texture/Image.png", camera);
	hitObject2_->SetPrimitive(std::move(cylinder2));
	hitObject2_->GetPrimitive()->SetPsoType(BasePrimitive::PsoType::kRingClamp);
	hitObject2_->SetIsDraw(false);
	hitObject2_->GetPrimitive()->GetMaterial()->color = provisionalData_.hit2Color;

	// 敵の位置捕捉
	enemyPos_.x = targetRange_.position.x + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.z = targetRange_.position.z + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
}

// 更新
void PlayerRangeBombingBullet::Update()
{
	// 出現させない
	emitterPoint->GetCommonData().emit = false;
	emitterPoint->GetWorldTransform().translate_ = object_->GetWorldTransform().worldMat_.GetWorldPosition();	// エミッター位置更新
	emitterPoint->GetCommonData().prevTranslate = object_->GetWorldTransform().worldPreMat_.GetWorldPosition(); // エミッター位置更新

	// 当たったら死ぬ
	if (Hit) {
		effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());
		isAlive_ = false;
	}
	
	// 生きているなら
	if (isAlive_) {
		Vector3 norm;
		switch (phase_)	// フェーズ
		{
		case 0:
			if (t > 0.0f) {}
			else {
				// 煙出す
				effect_->Emit("stratSmoke01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 10.0f,-5.0f,0.0f });
			}

			// カウント更新
			count += GetTimer() * provisionalData_.timeSpeedPhase0;

			// カウントをtに代入
			t = count;

			// 位置を線形補間
			object_->GetWorldTransform().translate_ = Lerp(str, randPosSky, t);

			// 移動方向の設定
			norm = randPosSky - str;
			velocity_ = norm * GetTimer();

			// tが1に達したら
			if (t >= 1) {
				t = 0;		// 初期化
				count = 0;	// 初期化
				phase_++;	// フェーズ移行
			}


			break;
		case 1:

			
			// カウント更新
			count += GetTimer();

			// カウントが最大値を達したら
			if (count >= max_count)
			{
				// 敵の位置に向かって落ちる
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();
				float posLength = Length(pos);

				// エミッターON
				emitterPoint->GetCommonData().emit = true;

				

				// 方向指定
				Vector3 pos2 = pos;
				velocity_ = pos2.Normalize() * provisionalData_.speedPhase1;
				velocity_.y = 0; // 垂直落下

				// オブジェクトの位置更新
				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();

				// 敵の範囲内に入ったら
				if (provisionalData_.distanceToEnemy >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++; // フェーズ移行
					posGround = enemyPos_; // 落ちる位置決定

					// 
					targetPos = posGround - object_->GetWorldPosition();
					count = 0;	// カウント初期化

					// ヒットオブジェクト描画
					hitObject2_->SetIsDraw(true);

					// エフェクト出現させない
					emitterPoint->GetCommonData().emit = false;
					emitterPoint->GetCommonData().lifeTime = 0.01f;
				}
				
			}
			break;
		case 2:
			// リングエフェクトの位置を設定
			effect_->Emit("ringEmit", object_->GetWorldTransform().worldMat_.GetWorldPosition());

			// カウント更新
			count += GetTimer();

			// カウントが最大値を達したら
			if (count >= max_count)
			{
				velocity_ = targetPos * provisionalData_.speedPhase2;	// 速度設定
				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();	// 位置更新
			}

			// 0.0f以下になったら
			if (-0.0f >= object_->GetWorldPosition().y) {

				// 位置を固定
				object_->GetWorldTransform().translate_.y = 0.5f;
				
				// 死亡
				isAlive_ = false;
				count = 0;	// カウント初期化
				phase_ = 0;	// フェーズ初期化
				
				isEffectPlay_ = true;	// エフェクト再生

				object_->GetWorldTransform().Update(); // オブジェクト位置更新
				
				// ヒットオブジェクト描画しない
				hitObject2_->SetIsDraw(false);


				// それぞれのパーティクル出現
				effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("missileHitCylinder", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,provisionalData_.hitCylinderY,0 });
				effect_->Emit("smokePlaneExpSmoke", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("AnimatedCube", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("expPlane01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,provisionalData_.expPlaneY,0 });
				effect_->Emit("expSpark", object_->GetWorldTransform().worldMat_.GetWorldPosition() );
			}

			break;
		}
		// ミサイル方向
		Vector3 velo = velocity_.Normalize();

		

		// Y軸周り角度(θy)
		object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Length(Vector3(velocity_.x, 0, velocity_.z));
		// X軸周り角度(θx)
		object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

		// ヒットオブジェクト位置と回転設定
		hitObject2_->GetWorldTransform().translate_ = posGround + Vector3{ 0,provisionalData_.hitObjectY,0 };
		hitObject2_->GetWorldTransform().rotate_.x = Math::DegreesToRadians(provisionalData_.hitObjectRotateX);
	}

	// トレイル発生
	if (trailTime_ > 0.0f) {
		object_->isEmitTrailEffect = true;
	}
	trailTime_ = 1;

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
}

// 描画
void PlayerRangeBombingBullet::Draw()
{

}

// エフェクト描画
void PlayerRangeBombingBullet::DrawP()
{
}

// 2D描画
void PlayerRangeBombingBullet::Draw2D()
{
}

// 移動中の煙初期化
void PlayerRangeBombingBullet::InitMoveSmoke()
{

}


