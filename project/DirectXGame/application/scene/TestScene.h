#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Camera/DebugCamera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/Emit/ParticleEmitter.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/engine/effect/Trail/TrailEffect.h"


#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


#include"DirectXGame/engine/Effect/Primitive/Primitive.h"
#include"DirectXGame/engine/SkyBox/SkyBox.h"

#include "DirectXGame/engine/2d/Primitive2D.h"
#include "DirectXGame/engine/collider/Octree/Octree.h"
#include "DirectXGame/engine/collider/2d/ColliderFanction2D.h"
#include "DirectXGame/engine/collider/3d/ColliderFanction3D.h"

#include "DirectXGame/engine/math/MapChip.h"
#include "DirectXGame/engine/math/AStarAlgorithm.h"
#include "DirectXGame/engine/math/Noise.h"
#include "DirectXGame/engine/Animation/AnimationData.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"
#include "DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include"DirectXGame/application/base/BaseClass/Camera/CameraManeger.h"

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

	void AppGlobalVariables();

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
	
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManeger> cameraManeger_;

	/// <summary>
	/// 3Dオブジェクト
	/// </summary>

	

	Object3d* skyBoxObject;
	Object3d* skyBoxObject2;
	Object3d* oceanObject;

	
	std::unique_ptr<SkyBox> skyBox;
	std::unique_ptr<SkyBox> skyBox2;


	// オーシャンシェーダー
	std::unique_ptr < Ocean> ocean_ = nullptr;



	/// <summary>
	/// スプライト
	/// </summary>
	
	std::vector<std::unique_ptr<Sprite>> sprite_;



	/// <summary>
	/// パーティクルエミッタ
	/// </summary>



	/// <summary>
	/// ライト
	/// </summary>

	std::shared_ptr<PointLight> point;

	std::shared_ptr<SpotLight> spot;

	std::shared_ptr<DirectionalLight> directional;

	/// <summary>
	/// その他
	/// </summary>
	
	

	std::unique_ptr<LoadLevelData> loadData_;

	bool g_bool = false;
	int g_int = 0;
	uint32_t g_uint = 0;
	float g_float = 0.0f;
	Vector2 g_v2 = {};
	Vector3 g_v3 = {};
	Vector4 g_v4 = {};
	std::string g_string = "name";
	Transform g_transform = {};


	bool g_aaaa = false;


	Vector3 velo;
};



