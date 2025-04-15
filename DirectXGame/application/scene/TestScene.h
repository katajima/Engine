#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Camera/DebugCamera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/base/TextureManager.h"
#include "DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include"DirectXGame/engine/base/ImGuiManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/engine/effect/Trail/TrailEffect.h"


#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


#include"DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/2d/Primitive2D.h"
#include "DirectXGame/engine/collider/Octree/Octree.h"
#include "DirectXGame/engine/collider/2d/ColliderFanction2D.h"
#include "DirectXGame/engine/collider/3d/ColliderFanction3D.h"

#include "DirectXGame/engine/math/MapChip.h"
#include "DirectXGame/engine/math/AStarAlgorithm.h"
#include "DirectXGame/engine/math/Noise.h"


class TestScene : public BaseScene
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

private: // 各初期化

	void InitializeObject3D(); // オブジェクト3D

	void InitializeObject2D(); // オブジェクト2D

	void InitializeParticle(); // パーティクル

	void InitializeLight(); // ライト

	void InitializeCamera(); //かめら

	void InitializeOthers(); // そのほか

private:

	

private:
	// 振るまい(シーンの部屋)
	enum class SceneBehavior {
		kSceneRoom01,   // 海、スカイボックス、スプライト
		kSceneRoom02,   // パーティクル
		kSceneRoom03,   // マルチマテリアルやマルチメッシュ
		kSceneRoom04,   // スキニング用
		kSceneRoom05,   // 当たり判定系
		kSceneRoom06,   
		kSceneRoom07,   
		kSceneRoom08,   
		kSceneRoom09,   
		kSceneRoom10,   
	};

	//振るまい
	SceneBehavior behavior_ = SceneBehavior::kSceneRoom01;
	// 次の振るまいリクエスト
	std::optional<SceneBehavior> behaviorRequest_ = std::nullopt;

private:

	// 各ルーム初期化
	void InitializeRoom01();
	void InitializeRoom02();
	void InitializeRoom03();
	void InitializeRoom04();
	void InitializeRoom05();
	void InitializeRoom06();
	void InitializeRoom07();
	void InitializeRoom08();
	void InitializeRoom09();
	void InitializeRoom10();

	// 更新
	void UpdateRoom01();
	void UpdateRoom02();
	void UpdateRoom03();
	void UpdateRoom04();
	void UpdateRoom05();
	void UpdateRoom06();
	void UpdateRoom07();
	void UpdateRoom08();

	// 部屋切り替え
	void SwitchRoom();

private:
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ImGuiManager* imGuiManager = nullptr;


	/// <summary>
	/// カメラ
	/// </summary>
	
	std::unique_ptr < Camera> camera;
	std::unique_ptr < DebugCamera> debugCamera;
	
	bool isDebugCamera = false;

	/// <summary>
	/// 3Dオブジェクト
	/// </summary>

	std::unique_ptr<Object3d> stairObject;
	Object3d skinningObject;
	Object3d skinningObject2;
	Object3d tail;
	Object3d multiy;

	std::unique_ptr<Object3d> playerObject;
	std::unique_ptr<Object3d> goalObject;


	// オーシャンシェーダー
	std::unique_ptr < Ocean> ocean_ = nullptr;

	/// <summary>
	/// スプライト
	/// </summary>
	
	std::vector<std::unique_ptr<Sprite>> sprite_;

	std::unique_ptr<Primitive2D> primitive2d1_;
	std::unique_ptr<Primitive2D> primitive2d2_;

	float inRad = 50;
	float outRad = 100;
	int segment = 5;

	/// <summary>
	/// パーティクルエミッタ
	/// </summary>

	std::unique_ptr<ParticleEmitter> emitter_  =nullptr;

	std::unique_ptr<ParticleEmitter> emitterEnemy_  =nullptr;

	std::unique_ptr<ParticleEmitter> primitvPlane_  =nullptr;



	/// <summary>
	/// ライト
	/// </summary>

	std::shared_ptr<PointLight> point;

	std::shared_ptr<SpotLight> spot;

	std::shared_ptr<DirectionalLight> directional;

	/// <summary>
	/// その他
	/// </summary>
	
	std::unique_ptr <Octree> octree;
	Vector3 div_ = { 1,1,1 };
	int maxDepth = { 1 };

	Capsule capsule_ = Capsule{Vector3{},Vector3{},10};
	Vector3 offset_ = { 0,0,0 };


	Triangle2D tri2d = { Vector2{} ,Vector2{} ,Vector2{}};
	Vector3 triCen = {0,0,0};

	Sphere2D sphere2d{};
	WorldTransform world{};

	ObjectInstans object_;

	std::unique_ptr<MapChip> map = std::make_unique<MapChip>(100, 100, 5.0f);
	std::vector<Vector2> path;  // 最短経路の結果を格納するためのベクター
	AStarPathfinder pathfinder;


	std::unique_ptr<Noise> noise = std::make_unique<Noise>();

};



