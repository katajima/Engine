#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include"DirectXGame/engine/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include <DirectXGame/application/base/Attack/AttackController.h>

#include"DirectXGame/application/base/Stage/MainStage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/engine/Camera/CameraManager.h"
#include"DirectXGame/application/base/Effect/Effect.h"


#include "DirectXGame/application/base/Input/InputCoordinator.h"
#include <DirectXGame/application/base/Game/GameFlowController.h>

#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include <DirectXGame/application/base/Attack/Combo/Base/Debug/ComboEditor.h>
#include <DirectXGame/application/base/Bullet/Base/Debug/ProjectileDebug.h>

#include <DirectXGame/application/base/Object/CollisionRegistrationSystem.h>

/// <summary>
/// CharacterDebugSceneを管理・実装するクラス。
/// </summary>
class CharacterDebugScene : public Engine::BaseScene {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレーム更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画3d
	/// </summary>
	void Draw3D() override;
	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;
private:
	/// <summary>
	/// 入力関係初期化
	/// </summary>
	void InitializeInput();
	/// <summary>
	/// キャラクター関係初期化
	/// </summary>
	void InitializeCharacter();
	/// <summary>
	/// カメラ関係初期化
	/// </summary>
	void InitializeCamera();
	/// <summary>
	/// エフェクト関係初期化
	/// </summary>
	void InitializeEffect();
	/// <summary>
	/// 基本的な初期化
	/// </summary>
	void InitializeBase();
	/// <summary>
	/// デバック関係初期化
	/// </summary>
	void InitializeDebug();
private:
	/// <summary>
	/// 入力関係
	/// </summary>
	void UpdateInput(float dt);
	/// <summary>
	/// キャラクター関係更新
	/// </summary>
	void UpdateCharacter(float dt);
	/// <summary>
	/// 基本的は更新
	/// </summary>
	void UpdateBase(float dt);
	/// <summary>
	/// デバッグ関係更新
	/// </summary>
	void UpdateDebug(float dt);
	/// <summary>
	/// ImGui更新
	/// </summary>
	void UpdateImGui();
	/// <summary>
	/// グローバルバリアブル適応
	/// </summary>
	void ApplyGlobalVariables() {};
	/// <summary>
	/// デバッグ対象設定
	/// </summary>
	void SetDebugTarget(uint32_t tag);

private:
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;

	// 物理入力、アクション入力、コマンドをまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;

	// ゲームの流れやルールの管理をするクラス
	std::unique_ptr<GameFlowController> gameFlowController_ = nullptr;
private:
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// 宇宙カメラ
	std::unique_ptr<UniverseCamera> universeCamera_;
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;
	// エフェクト
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;
	// エフェクト
	std::unique_ptr<EffectSystem> effect_;
	// スペシャルポイント管理クラス
	std::unique_ptr<SpecialPointManager> specialPointManager_ = nullptr;
private:
	// キャラクター管理
	std::unique_ptr<Character::CharacterManager> characterManager_;
	// ステージ
	std::unique_ptr<MainStage> stage_;
	// 弾
	std::unique_ptr<BulletManager> bulletManager_;
	// ヒットボックスシステム
	std::unique_ptr<HitBox::System> hitBoxSystem_;
	// シェイク
	CameraShakeData shakeData{};
	// ズーム
	CameraZoomData zoomData{};
private:
	// 衝突登録システム
	std::unique_ptr<CollisionRegistrationSystem> collisionRegistrationSystem_ = nullptr;
private:
	// ゲームUI
	std::unique_ptr<GameUI> gameUI;
private:
	// コンボエディター
	std::unique_ptr<Combo::Editor> comboEditor_;
	// 弾デバッグ
	std::unique_ptr<Projectile::ProjectileDebug> projectileDebug_;
	// デバック対象
	Character::BaseCharacter* debugTarget = nullptr;
	// タグ
	uint32_t tagNumber = 0;
	// HP
	float hp = 9999;
};
