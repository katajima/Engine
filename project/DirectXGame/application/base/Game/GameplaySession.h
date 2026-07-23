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
	/// 所有しているゲームプレイ共通リソースを破棄する。
	/// </summary>
	~GameplaySession() {
		/// <summary>
		/// シーン側からFinalizeが呼ばれなかった場合でもGPUリソースを残さない。
		/// </summary>
		Finalize();
	}

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
		const Vector3& playerPosition);

	/// <summary>
	/// 入力、カメラ、キャラクター、弾、衝突判定などの共通基盤を終了する。
	/// </summary>
	void Finalize();

	/// <summary>
	/// 物理入力とゲーム入力をまとめる入力管理を取得する。
	/// </summary>
	InputCoordinator* GetInputCoordinator() const { return inputCoordinator_.get(); }
	/// <summary>
	/// 追従カメラを取得する。
	/// </summary>
	FollowCamera* GetFollowCamera() const { return followCamera_.get(); }
	/// <summary>
	/// カメラ管理を取得する。
	/// </summary>
	CameraManager* GetCameraManager() const { return cameraManager_.get(); }
	/// <summary>
	/// エフェクト管理を取得する。
	/// </summary>
	EffectSystem* GetEffectSystem() const { return effectSystem_.get(); }
	/// <summary>
	/// 必殺技ポイント管理を取得する。
	/// </summary>
	SpecialPointManager* GetSpecialPointManager() const { return specialPointManager_.get(); }
	/// <summary>
	/// キャラクター管理を取得する。
	/// </summary>
	Character::CharacterManager* GetCharacterManager() const { return characterManager_.get(); }
	/// <summary>
	/// 弾管理を取得する。
	/// </summary>
	BulletManager* GetBulletManager() const { return bulletManager_.get(); }
	/// <summary>
	/// ヒットボックス管理を取得する。
	/// </summary>
	HitBox::System* GetHitBoxSystem() const { return hitBoxSystem_.get(); }
	/// <summary>
	/// 衝突対象登録システムを取得する。
	/// </summary>
	CollisionRegistrationSystem* GetCollisionRegistrationSystem() const { return collisionRegistrationSystem_.get(); }

private:
	// 物理入力、アクション入力、コマンドをまとめて管理する。
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;
	// ゲームプレイ共通の追従カメラ。
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	// シーンで利用するカメラを管理する。
	std::unique_ptr<CameraManager> cameraManager_ = nullptr;
	// ゲームプレイ中のエフェクトを管理する。
	std::unique_ptr<EffectSystem> effectSystem_ = nullptr;
	// 必殺技ポイントを管理する。
	std::unique_ptr<SpecialPointManager> specialPointManager_ = nullptr;
	// プレイヤーと敵キャラクターを管理する。
	std::unique_ptr<Character::CharacterManager> characterManager_ = nullptr;
	// シーン内の弾を管理する。
	std::unique_ptr<BulletManager> bulletManager_ = nullptr;
	// ヒットボックスを管理する。
	std::unique_ptr<HitBox::System> hitBoxSystem_ = nullptr;
	// 各管理クラスの衝突対象を登録する。
	std::unique_ptr<CollisionRegistrationSystem> collisionRegistrationSystem_ = nullptr;
};
