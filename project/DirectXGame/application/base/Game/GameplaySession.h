#pragma once

#include <memory>

#include "DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include "DirectXGame/application/base/Character/Base/CharacterManager.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"
#include "DirectXGame/application/base/Object/CollisionRegistrationSystem.h"
#include "DirectXGame/application/base/Special/Point/SpecialPoint.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

/// <summary>
/// ゲームプレイ系シーンで共通利用する基盤システムを所有し、一括初期化するクラス。
/// </summary>
class GameplaySession {
public:
	/// <summary>
	/// 入力、カメラ、キャラクター、弾、衝突判定などの共通基盤を初期化する。
	/// </summary>
	/// <param name="input">物理入力。</param>
	/// <param name="entityManager">エンティティ管理。</param>
	/// <param name="globalVariables">調整値管理。</param>
	/// <param name="playerType">生成するプレイヤー種別。</param>
	/// <param name="playerPosition">プレイヤーの初期座標。</param>
	void Initialize(Engine::Input* input, Engine::EntityManager* entityManager,
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

	// 物理入力とゲーム入力をまとめる入力管理を取得する。
	InputCoordinator* GetInputCoordinator() const { return inputCoordinator_.get(); }
	// 追従カメラを取得する。
	FollowCamera* GetFollowCamera() const { return followCamera_.get(); }
	// カメラ管理を取得する。
	CameraManager* GetCameraManager() const { return cameraManager_.get(); }
	// エフェクト管理を取得する。
	EffectSystem* GetEffectSystem() const { return effectSystem_.get(); }
	// 必殺技ポイント管理を取得する。
	SpecialPointManager* GetSpecialPointManager() const { return specialPointManager_.get(); }
	// キャラクター管理を取得する。
	Character::CharacterManager* GetCharacterManager() const { return characterManager_.get(); }
	// 弾管理を取得する。
	BulletManager* GetBulletManager() const { return bulletManager_.get(); }
	// ヒットボックス管理を取得する。
	HitBox::System* GetHitBoxSystem() const { return hitBoxSystem_.get(); }
	// 衝突対象登録システムを取得する。
	CollisionRegistrationSystem* GetCollisionRegistrationSystem() const { return collisionRegistrationSystem_.get(); }

private:
	// 物理入力、アクション入力、コマンドをまとめて管理する。
	std::unique_ptr<InputCoordinator> inputCoordinator_;
	// ゲームプレイ共通の追従カメラ。
	std::unique_ptr<FollowCamera> followCamera_;
	// シーンで利用するカメラを管理する。
	std::unique_ptr<CameraManager> cameraManager_;
	// ゲームプレイ中のエフェクトを管理する。
	std::unique_ptr<EffectSystem> effectSystem_;
	// 必殺技ポイントを管理する。
	std::unique_ptr<SpecialPointManager> specialPointManager_;
	// プレイヤーと敵キャラクターを管理する。
	std::unique_ptr<Character::CharacterManager> characterManager_;
	// シーン内の弾を管理する。
	std::unique_ptr<BulletManager> bulletManager_;
	// ヒットボックスを管理する。
	std::unique_ptr<HitBox::System> hitBoxSystem_;
	// 各管理クラスの衝突対象を登録する。
	std::unique_ptr<CollisionRegistrationSystem> collisionRegistrationSystem_;
};
