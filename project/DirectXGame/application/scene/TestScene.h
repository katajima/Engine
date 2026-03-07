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
#include "DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"
#include"DirectXGame/application/base/Camera/FixedCamera/FixedCamera.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"


/// <summary>
/// テストシーン
/// </summary>
class TestScene : public Engine::BaseScene
{
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
	// グローバルバリアブル適応
	void AppGlobalVariables();

private: // 各初期化
	// オブジェクト3D初期化
	void InitializeObject3D(); 
	// オブジェクト2D初期化
	void InitializeObject2D(); 
	// パーティクル初期化
	void InitializeParticle(); 
	// ライト初期化
	void InitializeLight(); 
	// カメラ初期化
	void InitializeCamera(); 
	// その他の初期化
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
	Engine::Input* input_ = nullptr;
	Engine::Audio* audio_ = nullptr;
	Engine::ImGuiManager* imGuiManager = nullptr;
	// インプットシステム
	std::unique_ptr<InputSystem> inputSystem_;


	/// <summary>
	/// カメラ
	/// </summary>
	
	// 固定カメラ
	std::unique_ptr <FixedCamera> fixedCamera_;
	// カメラ管理
	std::unique_ptr<CameraManager> cameraManeger_;

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



