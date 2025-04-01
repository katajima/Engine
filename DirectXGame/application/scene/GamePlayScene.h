#pragma once
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include "DirectXGame/engine/base/TextureManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/audio/Audio.h"
#include "DirectXGame/engine/3d/ModelManager.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/3d/Object3dCommon.h"
#include"DirectXGame/engine/3d/ModelCommon.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/base/LevelData.h"

#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/application/base/Enemy/Enemy.h"
#include"DirectXGame/application/base/Player/Player.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/FollowCamera/FollowCamera.h"

#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"

#include "DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/effect/Trail/TrailEffect.h"

#include "DirectXGame/application/base/Enemy/EnemyManager.h"

// ゲームプレイシーン
class GamePlayScene : public BaseScene
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw3D() override;




	void Draw2D() override;

	void UpdateImGui();

	// レベルデータ
	void LoadLevelData();

	void ApplyGlobalVariables();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();



private:

	void InitializeResources();
	void InitializeCamera();

	/// <summary>
	//衝突判定と応答
	/// </summary>
	//void ChekAllCollisions();

	
	void PlaceObjectsOnCurve(const std::vector<Vector3>& controlPoints, float spacing);
private:
	// 振るまい
	enum class Behavior {
		kPhase1,	// フェーズ１
		kPhase2,	// フェーズ２
	};

	//振るまい
	Behavior behavior_ = Behavior::kPhase1;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
	
	//
	void BehaviorPhase1Initialize();
	//
	void BehaviorPhase1Update();
	//
	void BehaviorPhase2Initialize();
	//
	void BehaviorPhase2Update();
	
private:
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	std::shared_ptr<DirectionalLight> directional;

	
	// カメラ
	std::unique_ptr < Camera> camera;
	Vector3 cameraR;
	Vector3 cameraT;
	
	Vector3 cameraDebugT;
	Vector3 cameraDebugR;
	

	bool flag = false;
	Object3d cameraObj_;

	// 建物オブジェクト

	std::vector<Vector3> warePos;

	std::vector < std::unique_ptr<Object3d>> warehouseObject;
	

	// オーシャンシェーダー
	std::unique_ptr < Ocean> ocean_ = nullptr;
	
	//追従カメラ
	//std::unique_ptr<FollowCamera> followCamera_;


	std::vector <Object3d*> objects;

	std::unique_ptr<Player> player_;

	std::unique_ptr<EnemyManager> enemyManager_ = nullptr;

	ParticleManager* particleManager_;


	std::unique_ptr < Object3d> tail;
	std::unique_ptr < Object3d> tail2;
	Object3d sky;

	TrailEffect* trailEffect_;
	bool flag33;

	int count = 0;

	int sceneCount = 0;
	int clock = 1;
	// 衝突マネージャ
	std::unique_ptr<CollisionManager> collisionManager_;

private:
	

	std::unique_ptr<ParticleEmitter> emit_;

	std::unique_ptr<ParticleEmitter> moveLimitEmitter_ = nullptr;
};

