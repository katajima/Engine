#include "GameplaySession.h"

void GameplaySession::Initialize(Engine::Input* input, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Character::PlayerType playerType,
	const Vector3& playerPosition) {
	// シーン入力をアクション入力へ変換する管理クラスを生成する。
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(input);

	// キャラクターや弾が共有するエフェクトシステムを生成する。
	effectSystem_ = std::make_unique<EffectSystem>();
	effectSystem_->Initialize(entityManager, globalVariables);

	// 全ゲームプレイシーンで使用する追従カメラを生成する。
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(inputCoordinator_->GetInputSystem(), entityManager, globalVariables, {});

	// 追従カメラを既定カメラとしてカメラ管理へ登録する。
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), entityManager, globalVariables);
	cameraManager_->AddCamera({ followCamera_.get(), true }, "followCamera");
	entityManager->GetObject3dInstanceManager()->SetCamera(cameraManager_->GetCamera());

	// キャラクターと弾が登録するヒットボックスを管理する。
	hitBoxSystem_ = std::make_unique<HitBox::System>();
	hitBoxSystem_->Initialize(entityManager);

	// シーン内に存在する弾を一括管理する。
	bulletManager_ = std::make_unique<BulletManager>();
	bulletManager_->Initialize(entityManager, globalVariables, nullptr);
	bulletManager_->SetEffect(effectSystem_.get());

	// 必殺技ポイントを一括管理する。
	specialPointManager_ = std::make_unique<SpecialPointManager>();
	specialPointManager_->Initialize(entityManager, globalVariables);

	// 共通システムを接続してキャラクター管理を初期化する。
	characterManager_ = std::make_unique<Character::CharacterManager>();
	characterManager_->Initialize(inputCoordinator_->GetInputSystem(), hitBoxSystem_.get(), entityManager,
		globalVariables, cameraManager_->GetCamera());
	characterManager_->SetEffect(effectSystem_.get());
	characterManager_->SetFollowCamera(followCamera_.get());
	characterManager_->SetBulletManager(bulletManager_.get());
	characterManager_->SetCameraManager(cameraManager_.get());
	characterManager_->SetSpecialPointManager(specialPointManager_.get());

	// 指定された座標をTransformの移動値へ設定してプレイヤーを生成する。
	Transform playerTransform{};
	playerTransform.translate = playerPosition;
	characterManager_->CreateCharacter(playerType, "", playerTransform);
	followCamera_->SetTarget(&characterManager_->GetPlayer()->GetObjectComponent()->GetWorldTransform());
	inputCoordinator_->SetOwner(characterManager_->GetPlayer());

	// 各管理クラスから毎フレームの衝突対象を収集するシステムを生成する。
	collisionRegistrationSystem_ = std::make_unique<CollisionRegistrationSystem>();
	collisionRegistrationSystem_->Initialize(globalVariables, entityManager->Get3DLineCommon(), hitBoxSystem_.get(),
		characterManager_.get(), specialPointManager_.get(), bulletManager_.get());

	// GPUパーティクルでも共通カメラを使用する。
	entityManager->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManager_->GetCamera());
}

void GameplaySession::Finalize() {
	// キャラクターや弾が持つカメラ参照を、カメラ本体より先に破棄する。
	collisionRegistrationSystem_.reset();
	hitBoxSystem_.reset();
	bulletManager_.reset();
	characterManager_.reset();
	specialPointManager_.reset();
	effectSystem_.reset();

	// カメラ管理が描画系へ渡した非所有ポインタを外してからカメラを破棄する。
	if (cameraManager_) {
		cameraManager_->Finalize();
		cameraManager_.reset();
	}
	// 登録元カメラが所有するEngine::Cameraを破棄する。
	followCamera_.reset();
	inputCoordinator_.reset();
}
