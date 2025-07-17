#include "Player.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "assert.h"


void Player::Initialize(Input* input,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager,GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;
	input_ = input;
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	CreateGroup("Player");
	
	

	// HP設定
	Parameters().HP.Initiaize(1000, 0, 1000, 0);
	HP() = 1000; // 初期HP設定

	Parameters().speed = 20.0f;// 移動速度設定

	// プレイヤー
	objectBase_ = entity3DManager_->CreateObject3D("PlayerBase", Object3d::ObjectModelType::kNormal, position, camera_);
	objectBase_->SetModel("AnimatedCube.gltf");
	//objectBase_->Update();
	objectBase_->InitColliderComponent();
	InitMoveComponent();
	objectBase_->InitRigidBodyComponent();

	GetColliderComponent()->SetHitReceiver(this);
	
	InitializeBaseAddItem();

	// SphereColliderを追加
	auto sphere = std::make_unique<SphereCollider>();
	sphere->tag = CollisionTag::Player;
	sphere->layer = CollisionLayer::Player;
	sphere->collisionMask = 0xFFFFFFFF;
	sphere->radius = 2.0f; // 半径を適宜設定
	
	GetColliderComponent()->AddCollider(std::move(sphere));



	// 衝突時のコールバック登録
	GetColliderComponent()->onHitCallback = [this](Collider* self, Collider* other) {
		auto* otherComponent = static_cast<ColliderComponent*>(other->owner);
		if (!otherComponent) return;

		uint32_t otherId = otherComponent->GetUniqueId();


		Vector3 pushVec;
		if (other->tag == CollisionTag::Enemy) {
			if (self->ResolveCollision(*other, pushVec)) {
				pushVec.y = 0; // Y軸方向の押し戻しは無効化（地面に沿った動きにするため）
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;

				}
				objectBase_->GetWorldTransform().Update();
			}
		}
		if (other->tag == CollisionTag::Wall) {
			if (self->ResolveCollision(*other, pushVec)) {
				if (other->isStatic) {
					// 相手が動かないなら自分だけ押し戻す
					objectBase_->GetWorldTransform().translate_ += pushVec;
				}
				else if (self->isStatic) {

				}
				else {
					// 双方が動く → 半分ずつ押し戻す（応用例）
					objectBase_->GetWorldTransform().translate_ += pushVec * 0.5f;

				}
				Velocity().y = 0;
				
				//acceleration_.y = 0;
				objectBase_->GetWorldTransform().Update();
			}
		}
		BaseEnemy* enemy = static_cast<BaseEnemy*>(otherComponent->GetHitReceiver());

		if (!enemy) return;
		if (enemy->GetBasicBehavior() == BasicBehavior::kAttack) {
			float nowTime = MyGame::NowTime(); // ← 時間取得関数（例）

			if (GetContactRecord().CheckHistory(otherId, nowTime, 1.0f)) {
				return; // クールタイム中のため無視
			}

			GetContactRecord().AddHistory(otherId, nowTime);

			AddDamage(10.0f);
			followCamera_->GetUniqueCamera()->SetShake(0.25f, { 0.1f,0.1f,0.1f });
		}
	};

	// スペシャル攻撃
	special_ = std::make_unique<RangeBombingSpecial>();
	special_->Initialize(entity3DManager, entity2DManager, camera_);
	special_->SetParent(&objectBase_->GetWorldTransform());
	special_->SetInput(input);
	RangeBombingSpecial* rengeSp = static_cast<RangeBombingSpecial*>(special_.get());
	rengeSp->SetRadius(100);
	rengeSp->SetReticleParent(&objectBase_->GetWorldTransform());
	rengeSp->Set(followCamera_, bulletManager_);

	// 武器
	weapon_ = std::make_unique<PlayerWeapon>();
	weapon_->Initialize(input_, entity3DManager_, nullptr, globalVariables_, {}, camera);
	weapon_->GetObject3D()->GetWorldTransform().parent_ = &objectBase_->GetWorldTransform();
	weapon_->GetObject3D()->GetWorldTransform().translate_ = { 0,0.5f,0.5f };
	weapon_->SetCharacter(this);


	// UI
	ui_ = std::make_unique<PlayerUI>();
	ui_->Initialize(entity2DManager);
	/// エフェクト関係
	effect_->Initialize(entity3DManager_, entity2DManager, camera_);
	// トレイルエフェクト
	effect_->SetTrailParent(weapon_->GetObject3D());
	

	ChangeState("Move");
}

void Player::Update()
{
	UpdateBaseGetValue(); //保存機能 基本値の更新

	// ステート
	state_->Update();
	
	if (GetHP() <= 0) {
		flags_.isAlive = false;
	}
	if (ImGui::Button("Idle")) {
		objectBase_->SetAnimetion("Idle", 0.3f);
	}
	if (ImGui::Button("Run")) {
		objectBase_->SetAnimetion("Run", 0.3f);
	}


#ifdef _DEBUG



	ImGui::Begin("Debug");
	ImGui::InputFloat("HP", &HP());
	if (ImGui::Button("SP")) {
		special_->SetGauge(100);
	}
	ImGui::Text(state_->GetName().c_str());
	int count = weapon_->GetComboData().GetCurrentComboCount();
	ImGui::InputInt("ComboCount", &count);
	count = weapon_->GetComboData().GetComboMaxCount();
	ImGui::InputInt("maxComboCount", &count);
	bool is = weapon_->GetComboData().isComboNext;
	ImGui::Checkbox("isComboNext", &is);
	is = Situations().isJumping;
	ImGui::Checkbox("Jumping", &is);
	is = weapon_->GetAttackInput().GetIsAttack();
	ImGui::Checkbox("isAttack", &is);
	is = weapon_->GetAttackInput().GetIsState();
	ImGui::Checkbox("isState", &is);
	is = objectBase_->GetRigidBodyComponent()->IsGravity();
	ImGui::Checkbox("IsGravity", &is);

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


	



	moveComponent_->AddMove(MyGame::GameTime(), GetAlive(),*objectBase_);

	moveComponent_->Landing(Situations().isJumping, *objectBase_->GetTransformComponent(),*objectBase_->GetRigidBodyComponent());

	// 重力
	//GravityUpdate(MyGame::GameTime(), Situations().isJumping, GetAlive());
	if (!Situations().isJumping) {
		Velocity() = { 0,0,0 };
	}

	if (!isCreativeMode) {
		// 移動制限
		LimitMove(-Vector3{ 200,200,200 }, Vector3{ 200,200,200 });
	}

	objectBase_->UpdateWorldTransform();


	// 必殺技
	special_->Update();
	// エフェクト
	effect_->Update();
	// ヒットデータの更新
	weapon_->GetHitData().Update(MyGame::GameTime()); // 武器のヒットデータ更新
	//武器更新
	weapon_->Update();
}

#pragma region Draw

void Player::DrawEffect()
{
	effect_->Draw();
}

void Player::Draw2D()
{
	ui_->SetHPBerSize(static_cast<float>(HP()));

	ui_->SetIsTextmax(special_->GetIsSpecial());
	ui_->SetIsTextRB(special_->GetIsSpecial());
	ui_->SetSpecialGaugeSize(static_cast<float>(special_->GetGauge()));

	ui_->Draw();
}


#pragma endregion //描画関係


#pragma region Move

void Player::Move()
{
	Situations().isMoving = false;
	Vector3 velo = GetVelocity();

	//if (input_->IsControllerConnected()) {


		velo.x = input_->GetGamePadLeftStick().x;
		velo.z = input_->GetGamePadLeftStick().y;


		if (velo.x != 0.0f || velo.z != 0.0f) {
			Situations().isMoving = true;
			// 入力方向を正規化
			velo = Normalize(velo);
			velo = Multiply(velo, Parameters().speed);


			// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
			Matrix4x4 cameraWorldMatrix = Inverse(followCamera_->GetUniqueCamera()->GetViewMatrix());

			// カメラの向きに基づいて移動方向をワールド座標系に変換
			Vector3 worldDirection = {
				velo.x * cameraWorldMatrix.m[0][0] + velo.z * cameraWorldMatrix.m[2][0],
				0.0f,
				velo.x * cameraWorldMatrix.m[0][2] + velo.z * cameraWorldMatrix.m[2][2]
			};

			velo = Multiply(Normalize(worldDirection), Parameters().speed);

			//// 移動ベクトルをカメラの角度だけ回転する
			//Matrix4x4 rotateMatrixY = MakeRotateYMatrix(camera_->transform_.rotate.y);
			//velocity_ = TransformNormal(velocity_, rotateMatrixY);
			//
			if (velo.Length() != 0) {
				objectBase_->GetWorldTransform().rotate_.y = std::atan2(velo.x, velo.z);
			}
			
		}
		else {
			
		}
	//}
	//{


	//	if (input_->IsPushKey(DIK_W)) {
	//		Velocity().z += 0.3f;
	//	}
	//	if (input_->IsPushKey(DIK_S)) {
	//		Velocity().z -= 0.3f;

	//	}
	//	if (input_->IsPushKey(DIK_A)) {
	//		Velocity().x -= 0.3f;

	//	}
	//	if (input_->IsPushKey(DIK_D)) {
	//		Velocity().x += 0.3f;
	//	}


	//	if (Velocity().x != 0.0f || Velocity().z != 0.0f) {
	//		// 入力方向を正規化
	//		Velocity() = Normalize(Velocity());
	//		Situations().isMoving = true;

	//		// カメラのビュー行列の逆行列（カメラのワールド変換行列）を取得
	//		Matrix4x4 cameraWorldMatrix = Inverse(followCamera_->GetUniqueCamera()->GetViewMatrix());

	//		// カメラの向きに基づいて移動方向をワールド座標系に変換
	//		Vector3 worldDirection = {
	//			GetVelocity().x * cameraWorldMatrix.m[0][0] + GetVelocity().z * cameraWorldMatrix.m[2][0],
	//			0.0f,
	//			GetVelocity().x * cameraWorldMatrix.m[0][2] + GetVelocity().z * cameraWorldMatrix.m[2][2]
	//		};

	//		Velocity() = Multiply(Normalize(worldDirection), Parameters().speed);
	//	}
	//}
	
	Velocity().x = velo.x;
	Velocity().z = velo.z;


}

void Player::Jump()
{
	if (Situations().isJumping) return; // ジャンプ中は無効化
	//if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y)) { // ジャンプボタンが押されたらジャンプ
		if (GetAlive()) {
			Situations().isJumping = true;
			//Velocity().y += 40.0f; // ジャンプ時の加速度を設定
			objectBase_->GetRigidBodyComponent()->AddForce({0,180.0f,0});
		}
	//}
}

#pragma endregion //移動関係

#pragma region MyRegion

void Player::LockOn(const std::vector<BaseEnemy*>& enemys)
{

	//if (basicbehavior_ == BasicBehavior::kSpecialAttack) {
	//	if (bulletSpecial_->GetPhese() == 0) {
	//		lockedOnEnemies.clear();
	//		int i = 0;

	//		for (int j = 0; j < enemys.size(); j++) {
	//			if (i >= MaxLockOn) {
	//				break; // 最大ロックオン数を超えたら抜ける
	//			}

	//			Vector2 posEne = enemys[j]->GetObject3D()->GetScreenPosition();
	//			Vector2 diff = Vector2{ 640,360 } - posEne;
	//			float length = diff.Length();

	//			if (length <= 300.0f && enemys[j]->GetAlive()) {
	//				enemys[j]->SetLockOn(true);
	//				lockedOnEnemies.push_back(enemys[j]);
	//				i++;
	//			}
	//			else {
	//				enemys[j]->SetLockOn(false);
	//			}
	//		}
	//	}
	//}
	//else {
	//	for (int j = 0; j < enemys.size(); j++) {
	//		enemys[j]->SetLockOn(false);
	//	}
	//}
}

void Player::ApplyGlobalVariables()
{

}

#pragma endregion // そのほか
