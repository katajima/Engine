#include "Player.h"
#include"DirectXGame/application/base/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "assert.h"


void Player::Initialize(Input* input, DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	colliderComponent_->SetLineCommon(entity3DManager->Get3DLineCommon());
	// プレイヤー本体のSphereColliderを作成
	auto playerCollider = std::make_unique<SphereCollider>();
	playerCollider->radius = 1.0f;
	playerCollider->tag = CollisionTag::Player;
	playerCollider->layer = CollisionLayer::Player;
	playerCollider->isStatic = true;
	/*playerCollider->collisionMask =
		(1 << static_cast<uint32_t>(CollisionLayer::Enemy)) |
		(1 << static_cast<uint32_t>(CollisionLayer::EnemyAttack));*/



	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->AddCollider(std::move(playerCollider));
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

	// 衝突時のコールバック登録
	colliderComponent_->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		uint32_t otherId = otherComponent->GetUniqueId();


		Vector3 pushVec;
		if (self->ResolveCollision(*other, pushVec)) {
			if (other->isStatic) {
				// 相手が動かないなら自分だけ押し戻す
				objectBase_.worldtransform_.translate_ += pushVec;
			}
			else if (self->isStatic) {
				// 自分が動かない → 相手だけが押し戻される（通常ここでは何もしない）
			}
			else {
				// 双方が動く → 半分ずつ押し戻す（応用例）
				objectBase_.worldtransform_.translate_ += pushVec * 0.5f;
				// ※相手のTransformも取得して -0.5f してあげると対称押し戻しが可能
			}

			objectBase_.Update();
		}

		// すでに当たっていたら無視
		if (contactRecord_.CheckHistory(otherComponent->GetUniqueId())) {
			return;
		}

		// 初接触として処理
		contactRecord_.AddHistory(otherComponent->GetUniqueId());

		AddDamege(10);
		followCamera_->GetViewProjection().SetShake(0.5f, {1,1,1});
	};



	entity3DManager_ = entity3DManager;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	camera_ = camera;
	dxCommon_ = dxcommon;

	// プレイヤー
	objectBase_.Initialize(entity3DManager);
	objectBase_.SetCamera(camera_);
	objectBase_.worldtransform_.translate_ = position;
	objectBase_.Update();
	objectBase_.SetName("PlayerBase");

	primitiveCylinder_ = std::make_unique<Primitive>();

	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = 5.0f;
	cylinderParam.innerRadius = reticleRad_;
	cylinderParam.outerRadius = reticleRad_;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;


	primitiveCylinder_->Initialize<ShapeParameter::Cylinder>(entity3DManager->GetPrimitiveCommon(), Primitive::ShapeType::Cylinder, cylinderParam, "resources/Texture/effect/gradationLine.png");
	primitiveCylinder_->SetPsoType(Primitive::PsoType::kNoCullRingClamp);
	// レティクル

	objectReticle_ = std::make_unique<Object3d>();
	objectReticle_->Initialize(entity3DManager,Object3d::ObjectType::kPrimitive);
	objectReticle_->SetCamera(camera_);
	objectReticle_->SetName("レティクルシリンダー");
	objectReticle_->SetPrimitive(primitiveCylinder_.get());
	objectReticle_->SetIsDraw(false);
	objectReticle_->worldtransform_.parent_ = &objectBase_.worldtransform_;
	objectReticle_->worldtransform_.rotate_.x = DegreesToRadians(-90);
	objectReticle_->worldtransform_.translate_ = { 0,2,100 };


	// 体
	objectBody_.Initialize(entity3DManager);
	objectBody_.SetCamera(camera_);
	objectBody_.SetModel("AnimatedCube.gltf");
	objectBody_.SetName("PlayerBody");
	objectBody_.worldtransform_.parent_ = &objectBase_.worldtransform_;


	// スペシャル攻撃
	bulletSpecial_ = std::make_unique<BulletSpecial>();
	bulletSpecial_->Initialize(entity3DManager, entity2DManager, camera_);
	bulletSpecial_->SetParent(&objectBase_.worldtransform_);

	rangeBombingSpecial_ = std::make_unique<RangeBombingSpecial>();
	rangeBombingSpecial_->Initialize(entity3DManager, entity2DManager, camera_);
	rangeBombingSpecial_->SetParent(&objectBase_.worldtransform_);

	weapon_ = std::make_unique<playerWeapon>();
	weapon_->Initialize(entity3DManager, camera);
	weapon_->GetObject3D().worldtransform_.parent_ = &objectBase_.worldtransform_;
	weapon_->GetObject3D().worldtransform_.translate_ = { 0,0.5f,0.5f };
	weapon_->SetOffset({ 0,5.0f,0.5f });
	weapon_->SetPlayer(this);


	// Factory
	playerAttackFactory_ = std::make_unique<PlayerAttackFactory>();

	// 攻撃マネージャー
	attackManager_ = std::make_unique<AttackManager>();
	attackManager_->Initialize(input_, playerAttackFactory_.get());


	// Transform 登録
	transformMap["Player"] = &objectBase_.worldtransform_;
	attackManager_->SetContext(input_, transformMap);

	// 攻撃ノードの登録（攻撃名・次の遷移・キャンセル条件など）
	AttackNode node{};
	node.data.attackType = AttackType::Blow;
	node.data.transformId = "Player";
	node.data.activeFrames = 60.0f;
	node.data.recoveryFrames = 10.0f;
	node.data.startupFrames = 60.0f;
	node.canCancelFunc = [] { return true; };
	node.nextNodeIds = { "Punch2" };

	attackManager_->RegisterAttackNode("Punch1", node);

	// もう一つの攻撃も登録例
	AttackNode node2{};
	node2.data.attackType = AttackType::Blow;
	node2.data.transformId = "Player";
	node.data.activeFrames = 60.0f;
	node.data.recoveryFrames = 10.0f;
	node.data.startupFrames = 60.0f;
	node2.canCancelFunc = [] { return true; };
	node2.nextNodeIds = {};

	attackManager_->RegisterAttackNode("Punch2", node2);


	// UI
	ui_->Initialize(entity2DManager);


	/// エフェクト関係
	effect_->Initialize(dxCommon_, entity3DManager_, entity2DManager, camera_);
	// トレイルエフェクト
	effect_->GetTrailEffect()->SetObject(&weapon_->GetObject3D());
	effect_->SetTrailEffectParent(&weapon_->GetObject3D());
	effect_->SetTrailParent(&weapon_->GetObject3D());
	// ダッシュ用エフェクト
	effect_->SetDashEmitterParent(weapon_->GetObject3D().worldtransform_);


}

void Player::Update()
{
	effect_->GetDashEmitter()->transform_.rotate_.y = objectBase_.worldtransform_.rotate_.y;


	if (isAlive) {
		if (behaviorRequest_) {
			// ふるまいを変更する
			behavior_ = behaviorRequest_.value();
			// 各ふるまいごとの初期化を実行
			switch (behavior_) {
			case Behavior::kRoot:
			default:
				weapon_->GetObject3D().SetIsDraw(false);

				BehaviorRootInitialize();
				isInvincible = false;
				break;
			case Behavior::kAttack:
				weapon_->GetObject3D().SetIsDraw(true);
				BehaviorAttackInitialize();
				isInvincible = true;
				break;
			case Behavior::kJump:
				weapon_->GetObject3D().SetIsDraw(false);
				break;
			case Behavior::kDie:
				weapon_->GetObject3D().SetIsDraw(false);
				BehaviorDieInitialize();
				break;
			}
			// ふるまいリクエストリセット
			behaviorRequest_ = std::nullopt;
		}
		switch (behavior_) {
		case Behavior::kRoot: // 通常行動更新
		default:
			BehaviorRootUpdate();
			break;
		case Behavior::kAttack: // 攻撃行動更新
			BehaviorAttackUpdate();
			break;
		case Behavior::kJump:
			break;
		case Behavior::kDie:
			BehaviorDieUpdate();
			break;
		}
	}



#ifdef _DEBUG
	if (isInvincible) {
		objectBody_.model->modelData.mesh[0]->material->color = { 0,0,1,1 };
	}
	else {
		objectBody_.model->modelData.mesh[0]->material->color = { 1,1,1,1 };
	}


	ImGui::Begin("Debug");
	if (ImGui::Button("SP")) {
		bulletSpecial_->SetGauge(100);
		rangeBombingSpecial_->SetGauge(100);
	}
	ImGui::End();

#endif // _DEBUG


	rangeBombingSpecial_->Update();
	//bulletSpecial_->Update();
	if (hp <= 0) {
		isAlive = false;
	}
	workAttack.hitTime -= MyGame::GameTime();
	if (workAttack.hitTime <= 0) {
		workAttack.hitCount = 0;
	}




	// 攻撃開始条件（例：ボタンを押したら）
	if (input_->IsTriggerKey(DIK_Z)) {
		attackManager_->AddAttack("Punch1");
	}

	attackManager_->Update(MyGame::GameTime());


#ifdef _DEBUG
	ImGui::Begin("trail");
	Vector3 min = weapon_->GetObject3D().GetMesh(0)->GetMin();
	ImGui::InputFloat3("min", &min.x);
	Vector3 max = weapon_->GetObject3D().GetMesh(0)->GetMax();
	ImGui::InputFloat3("max", &max.x);

	ImGui::InputInt("HP", &hp);


	ImGui::End();

	if (input_->IsTriggerKey(DIK_C)) {
		if (!isCreativeMode) {
			isCreativeMode = true;
		}
		else {
			isCreativeMode = false;
		}

	}

#endif // _DEBUG


	// 重力
	Gravity();
	if (!isCreativeMode) {
		// 移動制限
		LimitMove();
	}
	// エフェクト
	effect_->Update();
	//


	weapon_->Update();


	colliderComponent_->UpdateAll(objectBase_.worldtransform_);
}

#pragma region Draw

void Player::Draw()
{
	if (isAlive) {
		switch (behavior_) {
		case Behavior::kRoot: // 通常行動更新
		default:
			break;
		case Behavior::kAttack: // 攻撃行動更新
			//weapon_->Draw();

			break;
		case Behavior::kJump:
			break;
		case Behavior::kDie:
			//bulletSpecial_->Draw();
			break;
		}


		//objectBody_.Draw();
	}
}

void Player::DrawP()
{
	effect_->Draw();
}

void Player::Draw2D()
{
	ui_->SetHPBerSize(static_cast<float>(hp));
	ui_->SetIsTextmax(bulletSpecial_->GetIsSpecial());
	ui_->SetIsTextRB(bulletSpecial_->GetIsSpecial());
	ui_->SetSpecialGaugeSize(static_cast<float>(bulletSpecial_->GetGauge()));
	


	ui_->SetIsTextmax(rangeBombingSpecial_->GetIsSpecial());
	ui_->SetIsTextRB(rangeBombingSpecial_->GetIsSpecial());
	ui_->SetSpecialGaugeSize(static_cast<float>(rangeBombingSpecial_->GetGauge()));

	ui_->Draw();

}


#pragma endregion //描画関係


#pragma region Move

void Player::Move()
{
	speed = 20.0f;
	velocity_ = { 0,0,0 };
	isMove = false;



	if (input_->IsControllerConnected()) {


		velocity_.x = input_->GetGamePadLeftStick().x;
		velocity_.z = input_->GetGamePadLeftStick().y;


		if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
			isMove = true;
			// 入力方向を正規化
			velocity_ = Normalize(velocity_);
			velocity_ = Multiply(velocity_, speed);


			// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
			Matrix4x4 cameraWorldMatrix = Inverse(followCamera_->GetViewProjection().GetViewMatrix());

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velocity_.x * cameraWorldMatrix.m[0][0] + velocity_.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velocity_.x * cameraWorldMatrix.m[0][2] + velocity_.z * cameraWorldMatrix.m[2][2]
			};

			velocity_ = Multiply(Normalize(worldDirection), speed);

			//// 移動ベクトルをカメラの角度だけ回転する
			//Matrix4x4 rotateMatrixY = MakeRotateYMatrix(camera_->transform_.rotate.y);
			//velocity_ = TransformNormal(velocity_, rotateMatrixY);
			//
			if (velocity_.Length() != 0) {
				objectBase_.worldtransform_.rotate_.y = std::atan2(velocity_.x, velocity_.z);
			}


		}
	}
	else {


		if (input_->IsPushKey(DIK_W)) {
			velocity_.z += 0.3f;
		}
		if (input_->IsPushKey(DIK_S)) {
			velocity_.z -= 0.3f;

		}
		if (input_->IsPushKey(DIK_A)) {
			velocity_.x -= 0.3f;

		}
		if (input_->IsPushKey(DIK_D)) {
			velocity_.x += 0.3f;
		}


		if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
			// 入力方向を正規化
			velocity_ = Normalize(velocity_);
			isMove = true;

			// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
			Matrix4x4 cameraWorldMatrix = Inverse(followCamera_->GetViewProjection().GetViewMatrix());

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velocity_.x * cameraWorldMatrix.m[0][0] + velocity_.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velocity_.x * cameraWorldMatrix.m[0][2] + velocity_.z * cameraWorldMatrix.m[2][2]
			};

			velocity_ = Multiply(Normalize(worldDirection), speed);


		}
	}
	if (behavior_ == Behavior::kRoot || behavior_ == Behavior::kDie) {

	}

}

void Player::Gravity() {

	// 重力加速度
	const float kGravityAcceleration = 9.8f;

	// 加速度ベクトル
	float accelerationVector = -kGravityAcceleration; // 毎フレームのデルタ時間で重力を適用
	if (!isCreativeMode) {
		accelerationY_ += accelerationVector * MyGame::GameTime();
		// 加速する
		velocity_.y += accelerationY_;
	}
	AddMove();
	// 着地
	if (objectBase_.worldtransform_.translate_.y <= groundY) {
		objectBase_.worldtransform_.translate_.y = groundY;
		accelerationY_ = 0.0f;
		graVelo = 0;
		isJamp = false;
	}
}

void Player::AddMove()
{
	if (isAlive)
		objectBase_.worldtransform_.translate_ += velocity_ * MyGame::GameTime();
}

void Player::LimitMove()
{
	if (objectBase_.worldtransform_.translate_.x > moveLimit + 50) {
		objectBase_.worldtransform_.translate_.x = moveLimit + 50;
	}
	if (objectBase_.worldtransform_.translate_.x < -(moveLimit + 50)) {
		objectBase_.worldtransform_.translate_.x = -(moveLimit + 50);
	}
	if (objectBase_.worldtransform_.translate_.z > (moveLimit + 50)) {
		objectBase_.worldtransform_.translate_.z = (moveLimit + 50);
	}
	if (objectBase_.worldtransform_.translate_.z < -(moveLimit + 50)) {
		objectBase_.worldtransform_.translate_.z = -(moveLimit + 50);
	}
}

#pragma endregion //移動関係


#pragma region MyRegion


void Player::LockOn(const std::vector<BaseEnemy*>& enemys)
{

	if (behavior_ == Behavior::kDie) {
		if (bulletSpecial_->GetPhese() == 0) {
			lockedOnEnemies.clear();
			int i = 0;

			for (int j = 0; j < enemys.size(); j++) {
				if (i >= MaxLockOn) {
					break; // 最大ロックオン数を超えたら抜ける
				}

				Vector2 posEne = enemys[j]->GetObject3D()->GetScreenPosition();
				Vector2 diff = Vector2{ 640,360 } - posEne;
				float length = diff.Length();

				if (length <= 300.0f && enemys[j]->GetAlive()) {
					enemys[j]->SetLockOn(true);
					lockedOnEnemies.push_back(enemys[j]);
					i++;
				}
				else {
					enemys[j]->SetLockOn(false);
				}
			}
		}
	}
	else {
		for (int j = 0; j < enemys.size(); j++) {
			enemys[j]->SetLockOn(false);
		}
	}
}

void Player::ApplyGlobalVariables()
{

}

#pragma endregion // そのほか