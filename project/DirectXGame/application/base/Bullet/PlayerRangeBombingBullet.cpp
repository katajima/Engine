#include "PlayerRangeBombingBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"

#include "DirectXGame/engine/math/random.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

PlayerRangeBombingBullet::~PlayerRangeBombingBullet()
{
	effectComponent_->GetGpuParticleManager()->ClearEmitter("MoveEmitte_" + std::to_string(countIndex_));
	effectComponent_->RemoveEmitterAll();
	effectComponent_->RemovePrimitiveAll();
	effectComponent_->RemoveTrailEffectAll();
}
//
void PlayerRangeBombingBullet::Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	globalVariables_ = globalVariables;
	entity3DManager_ = entity3DManager;
	//// オブジェクト設定
	object_ = entity3DManager->CreateObject3D("playerbullet", ObjectModelType::kNormal, position, camera);
	object_->SetModel("player_bullet.obj");
	object_->UseTrailEffect("resources/Texture/Image.png", 0.15f, { 1.0f,1.0f,1.0f,1.0f }, { 0,0.5f,0 }, { 0,-0.5f,0 });
	object_->isEmitTrailEffect = true;
	object_->Update();
	object_->InitColliderComponent();
	

	// エフェクトコンポーネント
	effectComponent_ = std::make_unique<EffectComponent>();
	effectComponent_->Init(entity3DManager_,globalVariables_);


	// コライダ設定
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::PlayerAttack;
	sphere->layer = CollisionLayer::PlayerAttack;
	sphere->collisionMask = (1 << static_cast<uint32_t>(CollisionLayer::Enemy));
	sphere->radius = 10.0f; // 半径を適宜設定
	sphere->Disable(); // 初期状態では無効化
	object_->GetColliderComponent()->AddCollider(std::move(sphere));



	// 衝突時のコールバック登録
	object_->GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent || other->tag != CollisionTag::Enemy) return;
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		uint32_t otherId = otherComponent->GetUniqueId();




		float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

		if (object_->GetColliderComponent()->contactRecord_.CheckHistory(otherId)) {
			return; // クールタイム中のため無視
		}

		object_->GetColliderComponent()->contactRecord_.AddHistory(otherId, nowTime);
		enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
		enemy->AddDamage(parameter_.damege);
		enemy->Emit();
		};


	// Y軸周り角度(θy)
	object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
	float length = Length(Vector3(velocity_.x, 0, velocity_.z));
	// X軸周り角度(θx)
	object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);

	// ダメージ量
	parameter_.damege = 30;
	// 生存フラグ
	isAlive_ = true;
	// 行動フェーズ
	phase_ = 0;


	// 初期地点記録
	str = object_->GetWorldTransform().translate_;

	// ランダムな空中位置を設定
	randPosSky = str + Vector3{ -1000,0,0 };

	// 最大Y座標
	max_y = 600.0f; 

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
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;

	// シリンダー生成
	std::unique_ptr<CylinderPrimitive> cylinder2 = std::make_unique<CylinderPrimitive>();
	cylinder2->Data() = cylinderParam;
	cylinder2->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/effect/gradationLine.png");


	cylinderParam.height = 10.0f;
	cylinderParam.innerRadius = 12.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = true;

	// 当たった時のオブジェクト生成
	hitObject2_ = entity3DManager->CreatePrimitiveObject3D<CylinderPrimitive>("cylinder", "resources/Texture/Image.png", camera);
	hitObject2_->SetPrimitive(std::move(cylinder2));
	hitObject2_->GetPrimitive()->SetPsoType(BasePrimitive::PsoType::kRingClamp);
	hitObject2_->SetIsDraw(false);
	hitObject2_->GetPrimitive()->GetMaterial()->color = { 1.0f,0.0f,0.0f,0.5f };

	enemyPos_.x = targetRange_.position.x + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.z = targetRange_.position.z + Random::RandomFloat(-targetRange_.radius, targetRange_.radius);
	enemyPos_.y = 2.0f;
}

// 更新
void PlayerRangeBombingBullet::Update()
{
	emitterPoint->GetCommonData().emit = false;
	emitterPoint->GetWorldTransform().translate_ = object_->GetWorldTransform().worldMat_.GetWorldPosition();
	emitterPoint->GetCommonData().prevTranslate = object_->GetWorldTransform().worldPreMat_.GetWorldPosition();

	// 当たったら死ぬ
	if (Hit) {
		effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());
		isAlive_ = false;
	}
	
	if (isAlive_) {
		Vector3 norm;
		switch (phase_)
		{
		case 0:
			if (t > 0.0f) {
				flag_ = true;
			}
			else {
				effect_->Emit("stratSmoke01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 10.0f,-5.0f,0.0f });
			}

			count += GetTimer() * 2.0f;

			t = count;

			object_->GetWorldTransform().translate_ = Lerp(str, randPosSky, t);

			norm = randPosSky - str;

			velocity_ = norm * GetTimer();

			if (t >= 1) {
				t = 0;
				count = 0;
				phase_++;
			}


			break;
		case 1:

			flag_ = true;

			count += GetTimer();
			if (count >= max_count)
			{
				Vector3 pos = enemyPos_ - object_->GetWorldPosition();

				float posLength = Length(pos);

				emitterPoint->GetCommonData().emit = true;

				
				if (posLength > 500.0f) {
					emitterPoint->GetCommonData().count = 5;

				}
				else if (posLength > 100.0f) {
					emitterPoint->GetCommonData().count = 5;
				}
				else {
					emitterPoint->GetCommonData().count = 5;

				}

				Vector3 pos2 = pos;

				velocity_ = pos2.Normalize() * 1000;
				velocity_.y = 0;

				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();

				if (5 >= DistanceXZ(object_->GetWorldPosition(), enemyPos_)) {
					phase_++;
					posGround = enemyPos_;


					targetPos = posGround - object_->GetWorldPosition();
					count = 0;

					hitObject2_->SetIsDraw(true);
					emitterPoint->GetCommonData().emit = false;
					emitterPoint->GetCommonData().lifeTime = 0.01f;
				}
				
			}
			break;
		case 2:
			flag_ = true;
			// リングエフェクトの位置を設定
			effect_->Emit("ringEmit", object_->GetWorldTransform().worldMat_.GetWorldPosition());

	
			count += GetTimer();
			if (count >= max_count)
			{
				velocity_ = targetPos * 3;
				object_->GetWorldTransform().translate_ += velocity_ * GetTimer();
			}

			if (-0.0f >= object_->GetWorldPosition().y) {

				object_->GetWorldTransform().translate_.y = 0.5f;
				isAlive_ = false;
				count = 0;
				phase_ = 0;
				
				isEffectPlay_ = true;

				object_->GetWorldTransform().Update();
				
				flag_ = false;
				hitObject2_->SetIsDraw(false);


				// パーティクル出現
				effect_->Emit("missileHit", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("missileHitCylinder", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,10,0 });
				effect_->Emit("smokePlaneExpSmoke", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("AnimatedCube", object_->GetWorldTransform().worldMat_.GetWorldPosition());
				effect_->Emit("expPlane01", object_->GetWorldTransform().worldMat_.GetWorldPosition() + Vector3{ 0,5,0 });
				effect_->Emit("expSpark", object_->GetWorldTransform().worldMat_.GetWorldPosition() );
			}

			break;
		}
		// ミサイル方向
		Vector3 velo = velocity_.Normalize();

		// ミサイルとは反対方向に飛ばす
		emitterPoint->GetCommonData().velocity = -velo * 10.0f;


		// Y軸周り角度(θy)
		object_->GetWorldTransform().rotate_.y = std::atan2(velocity_.x, velocity_.z);
		float length = Length(Vector3(velocity_.x, 0, velocity_.z));
		// X軸周り角度(θx)
		object_->GetWorldTransform().rotate_.x = std::atan2(velocity_.y, -length);


		hitObject2_->GetWorldTransform().translate_ = posGround + Vector3{ 0,-6.0f,0 };
		hitObject2_->GetWorldTransform().rotate_.x = DegreesToRadians(-90);
	}



	// エフェクト再生
	if (isEffectPlay_) {
		time_ += GetTimer();
		object_->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, true);
		object_->SetIsDraw(false);

		if (time_ >= 0.5f) {
			object_->IsDelete();
			hitObject2_->IsDelete();
			isEffectPlay_ = false;
			object_->GetColliderComponent()->ClearColliders();
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
	effectComponent_->AddEmitter("smokePlane04", "smokePlane04", EmitterShapeType::LINE);
	//LineParticleEmitter* moveSmoke = effectComponent_->GetEmitterAs<LineParticleEmitter>("smokePlane04");

	effectComponent_->AddGPUParticleEmitter("MoveEmitte_" + std::to_string(countIndex_), "no3", EmitterType::Point);
	emitterPoint = effectComponent_->GetGpuParticleManager()->GetGpuParticleEmitter<GpuParticleEmitterPoint>("MoveEmitte_" + std::to_string(countIndex_));

	emitterPoint->GetCommonData().color = { 1,1,1 };
	emitterPoint->GetCommonData().frequency = 0.0f;
	emitterPoint->GetCommonData().count = 50;
	emitterPoint->GetCommonData().useBillboard = true;
	emitterPoint->GetCommonData().scale = { 5.8f ,5.8f ,5.8f };
	emitterPoint->GetCommonData().isAlhpa = true;
	emitterPoint->GetCommonData().rotateRange = DegreesToRadians(Vector3{ 180,180,180 });
	emitterPoint->GetCommonData().lifeTime = 0.4f;
	emitterPoint->GetCommonData().velocity = -velocity_;
	emitterPoint->GetCommonData().isScaling = true;
	emitterPoint->GetCommonData().scaleAmount = 1.25f;
	emitterPoint->GetCommonData().interpolation = EmitterInterpolation::Sequential;
	emitterPoint->GetCommonData().color = { 0.35f,0.35f,0.35f };
}


