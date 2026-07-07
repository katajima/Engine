#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/UniverseCamera/UniverseCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include <DirectXGame/application/base/Attack/AttackController.h>

#include"DirectXGame/application/base/Stage/MainStage.h"
#include"DirectXGame/application/base/UI/GameUI.h"
#include"DirectXGame/application/base/Bullet/Base/BulletManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include"DirectXGame/application/base/Effect/Effect.h"


#include "DirectXGame/application/base/Input/InputHander.h"
#include <DirectXGame/application/base/Input/InputManager.h>
#include <DirectXGame/application/base/Game/GameFlowController.h>

#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include <DirectXGame/application/base/Attack/Combo/Base/Debug/ComboEditor.h>
#include <DirectXGame/application/base/Bullet/Base/Debug/ProjectileDebug.h>

#include <DirectXGame/application/base/Object/CollisionRegistrationSystem.h>

class CharacterDebugScene : public Engine::BaseScene {
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画3d
	void Draw3D() override;
	// 描画2d
	void Draw2D() override;
private:
	// 入力関係初期化
	void InitializeInput();
	// キャラクター関係初期化
	void InitializeCharacter();
	// カメラ関係初期化
	void InitializeCamera();
	// エフェクト関係初期化
	void InitializeEffect();
	// 基本的な初期化
	void InitializeBase();
	// デバック関係初期化
	void InitializeDebug();
private:
	// 入力関係
	void UpdateInput(float dt);
	// キャラクター関係更新
	void UpdateCharacter(float dt);
	// 基本的は更新
	void UpdateBase(float dt);
	// デバッグ関係更新
	void UpdateDebug(float dt);
	// ImGui更新
	void UpdateImGui();
	// グローバルバリアブル適応
	void ApplyGlobalVariables();
	// デバッグ対象設定
	void SetDebugTarget(uint32_t tag);

private:
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;

	// インプットハンドラ
	std::unique_ptr <Character::InputHander> inputHander_;
	Character::ICommand* iCommand_;

	// インプットマネージャ
	std::unique_ptr<InputManager> inputManager_;

	// インプットシステム
	std::unique_ptr<InputSystem> inputSystem_;

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
	std::unique_ptr<SpecalPointManager> specalPointManager_ = nullptr;
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
