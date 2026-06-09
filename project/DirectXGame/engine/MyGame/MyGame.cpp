#include "MyGame.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/application/scene/SceneFactory.h"

const float Engine::MyGame::kDeltaTime_ = 1.0f / 60.0f;
float Engine::MyGame::kTimeSpeed_ = 1.0f;
float Engine::MyGame::nowTime = 0.0f;

void Engine::MyGame::SetTimeScale(float timeScale)
{
	// スロー演出などから呼ばれても破綻しないよう、時間倍率を安全な範囲に丸める
	if (timeScale < 0.0f) {
		timeScale = 0.0f;
	}
	if (timeScale > 4.0f) {
		timeScale = 4.0f;
	}

	kTimeSpeed_ = timeScale;
}

void Engine::MyGame::Initialize()
{
	debugTimer_.StartTimer();

	Framework::Initialize();


	// 最初のシーン
	sceneFactory_ = std::make_unique<SceneFactory>();
	// シーンマネージャー
	sceneManager_ = std::make_unique<SceneManager>();

	// シーンマネージャーに最初のシーンをセット
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->SetInput(input_.get());
	sceneManager_->SetWinApp(winApp_.get());
	sceneManager_->SetGlobalVariables(globalVariables_.get());
	sceneManager_->SetDirectXCommon(dxCommon_.get());
	sceneManager_->SetEntityManager(entityManager_.get());
	sceneManager_->Init();

	scenenames_.push_back("TEST");
	scenenames_.push_back("TITLE");
	scenenames_.push_back("GAMEPLAY");
	scenenames_.push_back("SELECT");
	scenenames_.push_back("SAMPLE");
	scenenames_.push_back("RESULT");
	scenenames_.push_back("TUTORIAL");
	scenenames_.push_back("CHARACTER");
	scenenames_.push_back("CUSTOM");

#ifdef _DEBUG
	sceneManager_->ChangeScene("CHARACTER");
#endif // _DEBUG
	sceneManager_->ChangeScene("TITLE");
	//sceneManager_->ChangeScene("GAMEPLAY");

	// リソース初期化
	InitializeResource();

	// グローバル変数の読み込み
	globalVariables_->LoadFiles();


	debugTimer_.EndTimer();
	debugTimer_.LogTimeSec("MyGameAllTime ");
}

void Engine::MyGame::Finalize()
{
	GpuParticleManager* gpuParticleManager_ = entityManager_->GetEffectManager()->GetGpuParticleManager();
	gpuParticleManager_->ClearEmitterAll();
	gpuParticleManager_->ClearGroupParticleAll();

	// 基底クラスの終了処理
	Framework::Finalize();
}

void Engine::MyGame::Update()
{
	entityManager_->Get3DLineCommon()->LineClear();
	// ImGuiの受付開始
	dxCommon_->GetImGuiManager()->Begin();

	Framework::Update();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
	nowTime += deltaTime;
	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}
	lastTime = currentTime;

#ifdef _DEBUG

	if(input_->IsTriggerKey(DIK_M)){
		entityManager_->GetEffectManager()->GetGpuParticleManager()->ClearEmitterAll();
	}
	if(input_->IsTriggerKey(DIK_N)){
		entityManager_->GetEffectManager()->GetGpuParticleManager()->ClearGroupParticleAll();
	}
	// FPS表示用ウィジェット
	if (!ImGui::Begin("File", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("Time")) {

		ImGui::Text("FPS: %.2f", fps);
		ImGui::DragFloat("TimeScale", &kTimeSpeed_, 0.01f);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scene")) {
		for(auto & name : scenenames_){
			if (ImGui::Button(name.c_str())) {
				sceneManager_->ChangeScene(name);
			}
		}
		ImGui::EndMenu();
	}

	
	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG

	// グローバル変数の更新
	globalVariables_->Update();

	dxCommon_->Update(sceneManager_.get(), entityManager_.get());




#ifdef _DEBUG
	dxCommon_->GetPostEffectManager()->RenderImGui();
	dxCommon_->RenderShadowMapDebugImGui();
#endif // _DEBUG
	// ImGuiの受付終了
	dxCommon_->GetImGuiManager()->End();
}

void Engine::MyGame::Draw()
{
	dxCommon_->Draw(sceneManager_.get(), entityManager_.get());
}



void Engine::MyGame::InitializeResource()
{
	TextureManager* textureManager = dxCommon_->GetTextureManager();
	ModelManager* modelManager = dxCommon_->GetModelManager();
	// テクスチャ全て読み込み
	textureManager->LoadAllTexturesInDirectory("resources/Texture/");
		
	// ModelData;
	LoadModel();


	/// <summary>
	/// パーティクル
	/// </summary>
	CreateParticle();
	

}

void Engine::MyGame::CreateParticle()
{
	ParticleManager* particleManager = entityManager_->GetEffectManager()->GetParticleManager();
	GpuParticleManager* gpuParticleManager_ = entityManager_->GetEffectManager()->GetGpuParticleManager();
	ModelManager* modelManager = dxCommon_->GetModelManager();
	PrimitiveCommon* primitiveCommon = entityManager_->GetPrimitiveCommon();

	primi = std::make_unique<TorusPrimitive>();
	primi->Initialize(primitiveCommon, "resources/Texture/uvChecker.dds");
	

	primiTrai = std::make_unique<TrianglePrimitive>();
	primiTrai->Initialize(primitiveCommon,"resources/Texture/Image.dds");

	primiPlane = std::make_unique<PlanePrimitive>();
	primiPlane->Initialize(primitiveCommon, "resources/Texture/uvChecker.dds");


	ShapeParameter::Star star;
	star.innerRadius = 1.0f;
	star.outerRadius = 7.0f;
	star.segments = 4;
	primiStar = std::make_unique<StarPrimitive>();
	primiStar->Initialize(primitiveCommon,"resources/Texture/Image.dds");
	primiStar->Data() = star;


	ShapeParameter::Ring ring;
	ring.segments = 16;
	ring.innerRadius = 0.1f;
	ring.outerRadius = 0.8f;


	primiRing = std::make_unique<RingPrimitive>();
	primiRing->Initialize(primitiveCommon,"resources/Texture/effect/gradationLine.dds");
	primiRing->Data() = ring;
	primiRing->MeshInitialize();


	particleManager->CreateParticleGroup("test", "resources/Texture/uvChecker.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("cc", "resources/Texture/Image.dds", modelManager->FindModel("plane.obj"), {}, EmitData::BlendType::MODE_ADD);
	particleManager->CreateParticleGroup("dustt", "resources/Texture/Image.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("primi", "resources/Texture/uvChecker.dds", primi.get());
	particleManager->CreateParticleGroup("primiPlane", "resources/Texture/effect/circle2.dds", primiPlane.get());


	particleManager->CreateParticleGroup("smoke", "resources/Texture/effect/aa.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("explosionSmoke", "resources/Texture/effect/aa.dds", modelManager->FindModel("plane.obj"));

	particleManager->CreateParticleGroup("dashEmit", "resources/Texture/effect/aa.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("moveLimit", "resources/Texture/Image.dds", modelManager->FindModel("plane.obj"));

	// 煙
	particleManager->CreateParticleGroup("smokePlane01", "resources/Texture/smoke/no1.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02", "resources/Texture/smoke/no2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03", "resources/Texture/smoke/no3.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane04", "resources/Texture/smoke/no4.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane05", "resources/Texture/smoke/no3.dds", primiPlane.get());
	particleManager->CreateParticleGroup("expPlane01", "resources/Texture/effect/exp2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("expSpark", "resources/Texture/effect/spark.dds", primiPlane.get());

	particleManager->CreateParticleGroup("smokePlane01_1", "resources/Texture/smoke/no1.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02_1", "resources/Texture/smoke/no2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03_1", "resources/Texture/smoke/no3.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane01_2", "resources/Texture/smoke/no1.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02_2", "resources/Texture/smoke/no2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03_2", "resources/Texture/smoke/no3.dds", primiPlane.get());
	particleManager->CreateParticleGroup("AnimatedCube", "resources/Texture/Image.dds", modelManager->FindModel("AnimatedCube.gltf"));

	particleManager->CreateParticleGroup("dust2", "resources/Texture/effect/dust2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("dust3", "resources/Texture/effect/dust4.dds", primiPlane.get());


	// 列車煙
	particleManager->CreateParticleGroup("trainDust", "resources/Texture/smoke/no3.dds", primiPlane.get());
	// 船の水しぶき
	particleManager->CreateParticleGroup("shipDust", "resources/Texture/effect/ink.dds", primiPlane.get());
	// 雲
	particleManager->CreateParticleGroup("cloudDust", "resources/Texture/effect/cloud.dds", primiPlane.get());
	particleManager->CreateParticleGroup("cloudDust2", "resources/Texture/effect/cloud2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("cloudDust3", "resources/Texture/effect/cloud3.dds", primiPlane.get());


	// ヒット
	particleManager->CreateParticleGroup("hitEffect", "resources/Texture/effect/effect.dds", primiPlane.get());
	particleManager->CreateParticleGroup("hitEffect2", "resources/Texture/effect/effect2.dds", primiPlane.get());
	particleManager->CreateParticleGroup("ringEmit", "resources/Texture/effect/ring.dds", primiPlane.get());
	particleManager->CreateParticleGroup("ringHit", "resources/Texture/effect/gradationLine.dds", primiRing.get()); // 柵
	particleManager->GetParticleGroups("ringHit").isUVClamp = true;
	particleManager->GetParticleGroups("ringHit").mesh->material->GetMaterialInstance().transform.scale.x = 10.0f;
	particleManager->GetParticleGroups("ringHit").mesh->material->GetMaterialInstance().transform.scale.y = 10.0f;


	// 敵関係
	particleManager->CreateParticleGroup("hitStar", "resources/Texture/Image.dds", primiStar.get());
	particleManager->CreateParticleGroup("hitTrai", "resources/Texture/Image.dds", primiTrai.get());
	particleManager->CreateParticleGroup("hit", "resources/Texture/effect/hit.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyGround", "resources/Texture/Image.dds", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyDust", "resources/Texture/effect/dust.dds", modelManager->FindModel("plane.obj"));

	// 敵部品
	particleManager->CreateParticleGroup("enemyTire", "resources/Texture/enemy.dds", modelManager->FindModel("enemyTire.obj")); // タイヤ
	particleManager->CreateParticleGroup("enemyDuct", "resources/Texture/enemy.dds", modelManager->FindModel("enemyDuct.obj")); // ダクト
	particleManager->CreateParticleGroup("enemyPlank", "resources/Texture/enemy.dds", modelManager->FindModel("enemyPlank.obj")); // 板
	particleManager->CreateParticleGroup("enemyGear", "resources/Texture/enemy.dds", modelManager->FindModel("enemyGear.obj")); // 歯車
	particleManager->CreateParticleGroup("enemyFence", "resources/Texture/enemy.dds", modelManager->FindModel("enemyFence.obj")); // 柵


	particleManager->CreateParticleGroup("dust", "resources/Texture/uvChecker.dds", modelManager->FindModel("plane.obj"));
	
	
	particleManager->CreateParticleGroup("cartridge", "resources/Texture/Image.dds", modelManager->FindModel("cartridge.obj"));


	// スクラップ部品
	particleManager->CreateParticleGroup("scrapBasis", "resources/Models/Scrap/Basis/color.png", modelManager->FindModel("basis.obj")); // タイヤ
	particleManager->CreateParticleGroup("scrapGear", "resources/Models/Scrap/Gear/color.png", modelManager->FindModel("gear.obj")); // ダクト
	particleManager->CreateParticleGroup("scrapIronRod", "resources/Models/Scrap/IronRod/color.png", modelManager->FindModel("ironRod.obj")); // 板
	particleManager->CreateParticleGroup("scrapScrew", "resources/Models/Scrap/Screw/color.png", modelManager->FindModel("screw.obj")); // 歯車
	particleManager->CreateParticleGroup("scrapTire", "resources/Models/Scrap/Tire/color.png", modelManager->FindModel("tire.obj")); // 柵


	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = 30.0f;
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;
	cylinder_ = std::make_unique<CylinderPrimitive>();
	cylinder_->Initialize(primitiveCommon, "resources/Texture/effect/gradationLine.dds");
	cylinder_->Data() = cylinderParam;
	cylinder_->MeshInitialize();
	// 
	particleManager->CreateParticleGroup("missileHitCylinder", "resources/Texture/effect/gradationLine.dds", cylinder_.get());
	particleManager->GetParticleGroups("missileHitCylinder").isUVClamp = true;
	

	// ミサイルHitエフェクト
	particleManager->CreateParticleGroup("missileHit", "resources/Texture/Image.dds", primiStar.get());

}

void Engine::MyGame::LoadModel()
{
	ModelManager* modelManager = dxCommon_->GetModelManager();

	
	/// <summary>
	/// テスト用フリーモデル
	/// </summary>
	modelManager->LoadModelAsync("multiMaterial.obj", "TestModel/multiMaterial");
	modelManager->LoadModelAsync("multiMaterial.gltf", "TestModel/multiMaterial");
	modelManager->LoadModelAsync("multiMesh.obj", "TestModel/multiMesh");
	modelManager->LoadModelAsync("plane.obj", "TestModel/plane");
	modelManager->LoadModelAsync("AnimatedCube.gltf", "TestModel/AnimatedCube");
	modelManager->LoadModelAsync("axis.obj", "TestModel/axis");
	modelManager->LoadModelAsync("teapot.obj", "TestModel/teapot");
	modelManager->LoadModelAsync("stair.obj", "TestModel");			// 階段



	/// <summary>
	/// 乗り物
	/// </summary>

	modelManager->LoadModelAsync("train.gltf",	"Vehicle/train");
	modelManager->LoadModelAsync("ship.gltf",	"Vehicle/ship");

	modelManager->LoadModelAsync("TrackCarBody.obj", "Vehicle/track");
	modelManager->LoadModelAsync("TrackCarTire.obj", "Vehicle/track");
	modelManager->LoadModelAsync("scrapBox.obj", "Vehicle/track");


	/// <summary>
	/// 地形
	/// </summary>
	modelManager->LoadModelAsync("renga.gltf", "stage/renga");				// 地形
	modelManager->LoadModelAsync("Ground.obj", "stage/Ground");				// 地形
	modelManager->LoadModelAsync("coast.gltf", "stage/terrain/coast");		// 地形
	modelManager->LoadModelAsync("black.obj", "stage/terrain/black");		// 地形(底)
	modelManager->LoadModelAsync("terrain.obj", "stage/terrain/terrain");	// 地形
	modelManager->LoadModelAsync("rail.obj", "stage/rail");					// 線路


	/// <summary>
	/// 建物
	/// </summary>
	modelManager->LoadModelAsync("stage.gltf", "stage"); // ステージ
	modelManager->LoadModelAsync("Missile.gltf", "stage/Missile"); // ミサイル発射台


	modelManager->LoadModelAsync("trainBridge.gltf", "stage/Bridge"); // ステージ(橋)
	modelManager->LoadModelAsync("field.obj", "stage/Field"); // フィールド()
	modelManager->LoadModelAsync("hasira.obj", "stage/objects"); // 柱()
	modelManager->LoadModelAsync("testField.obj", "stage/test"); // フィールド()

	modelManager->LoadModelAsync("testField.obj", "stage/Field"); // フィールド()

	modelManager->LoadModelAsync("lock.gltf", "stage/Stone"); // フィールド()


	modelManager->LoadModelAsync("Stone.obj", "stage/Title/Stone"); // フィールド()
	modelManager->LoadModelAsync("MoneyExchangePlace.obj", "stage/MoneyExchangePlace"); // フィールド(換金所)


	/// <summary>
	/// プレイヤー
	/// </summary>
	modelManager->LoadModelAsync("player_bullet.obj", "Bullet/player_bullet");	// プレイヤー弾
	modelManager->LoadModelAsync("cartridge.obj", "Bullet/player_bullet");	// プレイヤー弾

	modelManager->LoadModelAsync("Sword.obj", "Weapon/Sword");				// プレイヤー武器
	modelManager->LoadModelAsync("bulletWeapon.obj", "Weapon");

	modelManager->LoadModelAsync("origin.gltf", "Character/Player");		// プレイヤー
	modelManager->LoadModelAsync("testCharacter.gltf", "Character/Player");	// プレイヤー

	modelManager->LoadModelAsync("point.obj", "special");			// ポイント

	/// <summary>
	/// スプラップ
	/// </summary>
	modelManager->LoadModelAsync("basis.obj", "Scrap/Basis"); // スクラップ（基盤）
	modelManager->LoadModelAsync("gear.obj", "Scrap/Gear"); // スクラップ（歯車）
	modelManager->LoadModelAsync("ironRod.obj", "Scrap/IronRod"); // スクラップ（鉄の棒）
	modelManager->LoadModelAsync("screw.obj", "Scrap/Screw"); // スクラップ（ねじ）
	modelManager->LoadModelAsync("tire.obj", "Scrap/Tire"); // スクラップ（タイヤ）


	/// <summary>
	/// 敵
	/// </summary>

	modelManager->LoadModelAsync("enemy.obj",		"Character/enemyAll/enemy"); // 的
	modelManager->LoadModelAsync("enemy.gltf",		"Character/enemyAll/enemy2"); // 本体
	modelManager->LoadModelAsync("enemy2.obj",		"Character/enemyAll/enemy2"); // 本体
	modelManager->LoadModelAsync("enemyTire.obj",	"Character/enemyAll/tire"); // タイヤ
	modelManager->LoadModelAsync("enemyDuct.obj",	"Character/enemyAll/duct"); // ダクト
	modelManager->LoadModelAsync("enemyPlank.obj",	"Character/enemyAll/plank"); // 鋼板
	modelManager->LoadModelAsync("enemyGear.obj",	"Character/enemyAll/gear"); // 歯車
	modelManager->LoadModelAsync("enemyFence.obj",	"Character/enemyAll/fence"); // 柵


	modelManager->LoadModelAsync("enemyBullet.gltf", "Character/enemyAll/enemyBullet"); // 本体



	modelManager->LoadModelAsync("enemyBodySG01.obj",		"Character/enemy/enemySG01"); // 本体
	modelManager->LoadModelAsync("enemyWeaponSG01.obj",		"Character/enemy/enemySG01"); // 武器

	modelManager->LoadModelAsync("enemyBodySS01.obj",		"Character/enemy/enemySS01"); // 本体
	modelManager->LoadModelAsync("enemyPropellerSS01.obj",	"Character/enemy/enemySS01"); // プロペラ
	modelManager->LoadModelAsync("enemyMuzzleSS01.obj",		"Character/enemy/enemySS01"); // 武器
	

	// ここまでに投げた非同期モデル読み込みを待ち、GPU転送もまとめて完了させる。
	modelManager->WaitAllLoadFinished();
}
