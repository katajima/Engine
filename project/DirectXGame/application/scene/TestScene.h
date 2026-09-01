#pragma once
#include"DirectXGame/engine/scene/BaseScene.h"
#include"DirectXGame/engine/scene/SceneManager.h"
#include "DirectXGame/engine/input/Input.h"
#include <DirectXGame/engine/Audio/Audio.h>
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/audio/Audio.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/2d/SpriteCommon.h"
#include"DirectXGame/engine/2d/Sprite.h"

#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>


#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"



#include"DirectXGame/engine/SkyBox/SkyBox.h"


#include "DirectXGame/engine/collider/Octree/Octree.h"
#include "DirectXGame/engine/collider/2d/ColliderFunction2D.h"
#include "DirectXGame/engine/collider/3d/ColliderFunction3D.h"

#include "DirectXGame/engine/math/MapChip.h"
#include "DirectXGame/engine/math/AStarAlgorithm.h"
#include "DirectXGame/engine/math/Noise.h"
#include "DirectXGame/engine/Animation/AnimationData.h"
#include "DirectXGame/engine/base/Load/LoadLevelData.h"
#include "DirectXGame/engine/GlobalVariables/GlobalVariables.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include"DirectXGame/engine/Camera/CameraManager.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"
#include "DirectXGame/engine/Animation/AnimationComponent.h"

/// <summary>
/// テストシーン
/// </summary>
class TestScene : public Engine::BaseScene
{
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
	/// <summary>
	/// グローバルバリアブル適応
	/// </summary>
	void AppGlobalVariables();

private: // 各初期化
	/// <summary>
	/// オブジェクト3D初期化
	/// </summary>
	void InitializeObject3D(); 
	/// <summary>
	/// オブジェクト2D初期化
	/// </summary>
	void InitializeObject2D(); 
	/// <summary>
	/// パーティクル初期化
	/// </summary>
	void InitializeParticle(); 
	/// <summary>
	/// ライト初期化
	/// </summary>
	void InitializeLight(); 
	/// <summary>
	/// カメラ初期化
	/// </summary>
	void InitializeCamera(); 
	/// <summary>
	/// その他の初期化
	/// </summary>
	void InitializeOthers();

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

	/// <summary>
	/// 各ルーム初期化
	/// </summary>
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

	/// <summary>
	/// 更新
	/// </summary>
	void UpdateRoom01();
	void UpdateRoom02();
	void UpdateRoom03();
	void UpdateRoom04();
	void UpdateRoom05();
	void UpdateRoom06();
	void UpdateRoom07();
	void UpdateRoom08();

	/// <summary>
	/// 部屋切り替え
	/// </summary>
	void SwitchRoom();

private:
	Engine::Input* input_ = nullptr;
	Engine::ImGuiManager* imGuiManager = nullptr;
	// シーン内の入力をまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;


	/// <summary>
	/// カメラ
	/// </summary>
	
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_;

	/// <summary>
	/// 3Dオブジェクト
	/// </summary>


	std::vector<Engine::Object3d*> skinObjects;

	Engine::Object3d* skinObject;
	Engine::Object3d* skinObject2;


	Engine::Object3d* skyBoxObject;
	Engine::Object3d* skyBoxObject2;
	Engine::Object3d* oceanObject;

	
	std::unique_ptr<Engine::SkyBox> skyBox;
	std::unique_ptr<Engine::SkyBox> skyBox2;


	// オーシャンシェーダー
	std::unique_ptr < Engine::Ocean> ocean_ = nullptr;



	/// <summary>
	/// スプライト
	/// </summary>
	
	std::vector<std::unique_ptr<Engine::Sprite>> sprite_;



	/// <summary>
	/// パーティクルエミッタ
	/// </summary>



	/// <summary>
	/// ライト
	/// </summary>

	std::shared_ptr<Engine::PointLight> point;

	std::shared_ptr<Engine::SpotLight> spot;

	std::shared_ptr<Engine::DirectionalLight> directional;

	/// <summary>
	/// その他
	/// </summary>
	
	

	std::unique_ptr<LoadLevelData> loadData_;
};



