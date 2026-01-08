#include "MyGame.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/application/scene/SceneFactory.h"

const float Engine::MyGame::kDeltaTime_ = 1.0f / 60.0f;
float Engine::MyGame::kTimeSpeed_ = 1.0f;
float Engine::MyGame::nowTime = 0.0f;

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
	sceneManager_->SetGlobalVariables(globalVariables_.get());
	sceneManager_->SetDirectXCommon(dxCommon.get());
	sceneManager_->SetEntity3DManager(entity3DManager_.get());
	sceneManager_->SetEntity2DManager(entity2DManager_.get());
	sceneManager_->Init();
	sceneManager_->ChangeScene("TITLE");

	// リソース初期化
	InitializeResource();

	// グローバル変数の読み込み
	globalVariables_->LoadFiles();


	debugTimer_.EndTimer();
	debugTimer_.LogTimeSec("MyGameAllTime ");
}

void Engine::MyGame::Finalize()
{
	GpuParticleManager* gpuParticleManager_ = entity3DManager_->GetEffectManager()->GetGpuParticleManager();
	gpuParticleManager_->ClearEmitterAll();
	gpuParticleManager_->ClearGroupParticleAll();

	// 基底クラスの終了処理
	Framework::Finalize();
}

void Engine::MyGame::Update()
{
	entity3DManager_->Get3DLineCommon()->LineClear();
	// ImGuiの受付開始
	dxCommon->GetImGuiManager()->Begin();

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
		entity3DManager_->GetEffectManager()->GetGpuParticleManager()->ClearEmitterAll();
	}
	if(input_->IsTriggerKey(DIK_N)){
		entity3DManager_->GetEffectManager()->GetGpuParticleManager()->ClearGroupParticleAll();
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
	ImGui::EndMenuBar();
	ImGui::End();

#endif // _DEBUG

	// グローバル変数の更新
	globalVariables_->Update();

	dxCommon->Update(sceneManager_.get(), entity3DManager_.get());




#ifdef _DEBUG
	dxCommon->GetPostEffectManager()->RenderImGui();
#endif // _DEBUG
	// ImGuiの受付終了
	dxCommon->GetImGuiManager()->End();
}

void Engine::MyGame::Draw()
{
	dxCommon->Draw(sceneManager_.get(), entity3DManager_.get());
}



void Engine::MyGame::InitializeResource()
{
	TextureManager* textureManager = dxCommon->GetTextureManager();
	ModelManager* modelManager = dxCommon->GetModelManager();
	

	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/simasima.png");


	textureManager->LoadTexture("resources/Texture/grass.png");
	textureManager->LoadTexture("resources/Texture/ground.png");
	textureManager->LoadTexture("resources/Texture/icon/LockOnW.png");
	textureManager->LoadTexture("resources/Texture/renga.png");
	textureManager->LoadTexture("resources/Texture/enemy.png");

	textureManager->LoadTexture("resources/Texture/text/normalAttack.png");
	textureManager->LoadTexture("resources/Texture/text/special.png");
	textureManager->LoadTexture("resources/Texture/text/Hit.png");
	textureManager->LoadTexture("resources/Texture/text/HP.png");
	textureManager->LoadTexture("resources/Texture/text/SP.png");
	textureManager->LoadTexture("resources/Texture/text/wave.png");
	textureManager->LoadTexture("resources/Texture/text/seconds.png");




	textureManager->LoadTexture("resources/Texture/effect/aa.png");
	textureManager->LoadTexture("resources/Texture/effect/aa2.png");
	for (int i = 0; i < 10; i++) {
		std::string label = "resources/Texture/num/" + std::to_string(i) + ".png";
		textureManager->LoadTexture(label);
	}
	textureManager->LoadTexture("resources/Texture/num/Number_x64y96.png");


	textureManager->LoadTexture("resources/Texture/text/max.png");

	
	textureManager->LoadTexture("resources/Texture/effect/circle2.png");
	textureManager->LoadTexture("resources/Texture/effect/gradationLine.png");
	textureManager->LoadTexture("resources/Texture/effect/dust.png");
	textureManager->LoadTexture("resources/Texture/effect/hit.png");
	textureManager->LoadTexture("resources/Texture/effect/ring.png");

	// 煙
	textureManager->LoadTexture("resources/Texture/smoke/no1.png");
	textureManager->LoadTexture("resources/Texture/smoke/no2.png");
	textureManager->LoadTexture("resources/Texture/smoke/no3.png");
	
	//
	textureManager->LoadTexture("resources/Texture/effect/exp.png");
	
	textureManager->LoadTexture("resources/Texture/effect/spark.png");
	
	// XBOXコントローラボタンテクスチャ全て読み込み
	textureManager->LoadAllTexturesInDirectory("resources/Texture/XBOX");

	// アイコン全て読み込み
	textureManager->LoadAllTexturesInDirectory("resources/Texture/icon");
	
	// ModelData;
	LoadModel();


	/// <summary>
	/// パーティクル
	/// </summary>
	CreateParticle();
	

}

void Engine::MyGame::CreateParticle()
{
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();
	GpuParticleManager* gpuParticleManager_ = entity3DManager_->GetEffectManager()->GetGpuParticleManager();
	ModelManager* modelManager = dxCommon->GetModelManager();


	primi = std::make_unique<TorusPrimitive>();
	primi->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/uvChecker.png");
	

	primiTrai = std::make_unique<TrianglePrimitive>();
	primiTrai->Initialize(entity3DManager_->GetPrimitiveCommon(),"resources/Texture/Image.png");

	primiPlane = std::make_unique<PlanePrimitive>();
	primiPlane->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/uvChecker.png");


	ShapeParameter::Star star;
	star.innerRadius = 1.0f;
	star.outerRadius = 7.0f;
	star.segments = 4;
	primiStar = std::make_unique<StarPrimitive>();
	primiStar->Initialize(entity3DManager_->GetPrimitiveCommon(),"resources/Texture/Image.png");
	primiStar->Data() = star;


	ShapeParameter::Ring ring;
	ring.segments = 16;
	ring.innerRadius = 0.1f;
	ring.outerRadius = 0.8f;


	primiRing = std::make_unique<RingPrimitive>();
	primiRing->Initialize(entity3DManager_->GetPrimitiveCommon(),"resources/Texture/effect/gradationLine.png");
	primiRing->Data() = ring;
	primiRing->MeshInitialize();


	particleManager->CreateParticleGroup("test", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));

	particleManager->CreateParticleGroup("cc", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"), {}, EmitData::BlendType::MODE_ADD);

	particleManager->CreateParticleGroup("dustt", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));


	particleManager->CreateParticleGroup("primi", "resources/Texture/uvChecker.png", primi.get());

	particleManager->CreateParticleGroup("primiPlane", "resources/Texture/effect/circle2.png", primiPlane.get());


	particleManager->CreateParticleGroup("smoke", "resources/Texture/effect/aa.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("explosionSmoke", "resources/Texture/effect/aa.png", modelManager->FindModel("plane.obj"));

	particleManager->CreateParticleGroup("dashEmit", "resources/Texture/effect/aa.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("moveLimit", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));

	// 煙
	particleManager->CreateParticleGroup("smokePlane01", "resources/Texture/smoke/no1.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02", "resources/Texture/smoke/no2.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03", "resources/Texture/smoke/no3.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane04", "resources/Texture/smoke/no4.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane05", "resources/Texture/smoke/no3.png", primiPlane.get());
	particleManager->CreateParticleGroup("expPlane01", "resources/Texture/effect/exp2.png", primiPlane.get());
	particleManager->CreateParticleGroup("expSpark", "resources/Texture/effect/spark.png", primiPlane.get());

	particleManager->CreateParticleGroup("smokePlane01_1", "resources/Texture/smoke/no1.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02_1", "resources/Texture/smoke/no2.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03_1", "resources/Texture/smoke/no3.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane01_2", "resources/Texture/smoke/no1.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane02_2", "resources/Texture/smoke/no2.png", primiPlane.get());
	particleManager->CreateParticleGroup("smokePlane03_2", "resources/Texture/smoke/no3.png", primiPlane.get());

	particleManager->CreateParticleGroup("AnimatedCube", "resources/Texture/Image.png", modelManager->FindModel("AnimatedCube.gltf"));

	// 列車煙
	particleManager->CreateParticleGroup("trainDust", "resources/Texture/smoke/no3.png", primiPlane.get());
	// 船の水しぶき
	particleManager->CreateParticleGroup("shipDust", "resources/Texture/effect/ink.png", primiPlane.get());
	// 雲
	particleManager->CreateParticleGroup("cloudDust", "resources/Texture/effect/cloud.png", primiPlane.get());
	particleManager->CreateParticleGroup("cloudDust2", "resources/Texture/effect/cloud2.png", primiPlane.get());
	particleManager->CreateParticleGroup("cloudDust3", "resources/Texture/effect/cloud3.png", primiPlane.get());


	// ヒット
	particleManager->CreateParticleGroup("hitEffect", "resources/Texture/effect/effect.png", primiPlane.get());
	particleManager->CreateParticleGroup("hitEffect2", "resources/Texture/effect/effect2.png", primiPlane.get());
	particleManager->CreateParticleGroup("ringEmit", "resources/Texture/effect/ring.png", primiPlane.get());
	particleManager->CreateParticleGroup("ringHit", "resources/Texture/effect/gradationLine.png", primiRing.get()); // 柵
	particleManager->GetParticleGroups("ringHit").isUVClamp = true;
	particleManager->GetParticleGroups("ringHit").mesh->material->transform.scale.x = 10.0f;
	particleManager->GetParticleGroups("ringHit").mesh->material->transform.scale.y = 10.0f;


	// 敵関係
	particleManager->CreateParticleGroup("hitStar", "resources/Texture/Image.png", primiStar.get());
	particleManager->CreateParticleGroup("hitTrai", "resources/Texture/Image.png", primiTrai.get());
	particleManager->CreateParticleGroup("hit", "resources/Texture/effect/hit.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyGround", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyDust", "resources/Texture/effect/dust.png", modelManager->FindModel("plane.obj"));

	// 敵部品
	particleManager->CreateParticleGroup("enemyTire", "resources/Texture/enemy.png", modelManager->FindModel("enemyTire.obj")); // タイヤ
	particleManager->CreateParticleGroup("enemyDuct", "resources/Texture/enemy.png", modelManager->FindModel("enemyDuct.obj")); // ダクト
	particleManager->CreateParticleGroup("enemyPlank", "resources/Texture/enemy.png", modelManager->FindModel("enemyPlank.obj")); // 板
	particleManager->CreateParticleGroup("enemyGear", "resources/Texture/enemy.png", modelManager->FindModel("enemyGear.obj")); // 歯車
	particleManager->CreateParticleGroup("enemyFence", "resources/Texture/enemy.png", modelManager->FindModel("enemyFence.obj")); // 柵


	particleManager->CreateParticleGroup("dust", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));
	
	
	particleManager->CreateParticleGroup("cartridge", "resources/Texture/Image.png", modelManager->FindModel("cartridge.obj"));

	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = 30.0f;
	cylinderParam.innerRadius = 6.0f;
	cylinderParam.outerRadius = 12.0f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;
	



	cylinder_ = std::make_unique<CylinderPrimitive>();
	cylinder_->Initialize(entity3DManager_->GetPrimitiveCommon(), "resources/Texture/effect/gradationLine.png");
	cylinder_->Data() = cylinderParam;
	cylinder_->MeshInitialize();
	// 
	particleManager->CreateParticleGroup("missileHitCylinder", "resources/Texture/effect/gradationLine.png", cylinder_.get());
	particleManager->GetParticleGroups("missileHitCylinder").isUVClamp = true;
	

	// ミサイルHitエフェクト
	particleManager->CreateParticleGroup("missileHit", "resources/Texture/Image.png", primiStar.get());

}

void Engine::MyGame::LoadModel()
{
	ModelManager* modelManager = dxCommon->GetModelManager();

	modelManager->LoadModel("a.obj");

	modelManager->LoadModel("multiMaterial.obj", "multiMaterial");

	modelManager->LoadModel("multiMaterial.gltf", "multiMaterial");

	modelManager->LoadModel("multiMesh.obj", "multiMesh");


	modelManager->LoadModel("origin.gltf", "Character");
	modelManager->LoadModel("bulletWeapon.obj", "Weapon");



	modelManager->LoadModel("player_bullet.obj", "player_bullet");
	modelManager->LoadModel("cartridge.obj", "player_bullet");
	modelManager->LoadModel("Sword.obj", "Sword");
	modelManager->LoadModel("plane.obj", "plane");
	modelManager->LoadModel("AnimatedCube.gltf", "AnimatedCube");

	modelManager->LoadModel("Ground.obj", "Ground");
	modelManager->LoadModel("stair.obj");
	modelManager->LoadModel("BoxBox.obj", "BoxAABB");
	modelManager->LoadModel("point.obj", "special");



	modelManager->LoadModel("plane.obj", "plane");
	modelManager->LoadModel("axis.obj", "axis");


	modelManager->LoadModel("teapot.obj", "teapot");



	modelManager->LoadModel("rail.obj", "rail");
	modelManager->LoadModel("Sphere.obj", "sphere");
	modelManager->LoadModel("Sphere2.obj", "sphere");
	modelManager->LoadModel("Sphere3.obj", "sphere");
	modelManager->LoadModel("skydome.obj", "skydome");



	/// <summary>
	/// 乗り物
	/// </summary>

	modelManager->LoadModel("train.gltf", "train");
	modelManager->LoadModel("ship.gltf", "ship");

	/// <summary>
	/// 地形
	/// </summary>
	modelManager->LoadModel("renga.gltf", "renga");


	modelManager->LoadModel("coast.gltf", "terrain/coast");
	modelManager->LoadModel("black.obj", "terrain/black");
	modelManager->LoadModel("terrain.obj", "terrain/terrain");
	modelManager->LoadModel("stair.obj");



	/// <summary>
	/// 建物
	/// </summary>
	modelManager->LoadModel("building.obj", "buildingAll/building"); // ビル
	modelManager->LoadModel("warehouse.gltf", "buildingAll/warehouse"); // 倉庫
	modelManager->LoadModel("stage.gltf", "stage"); // ステージ
	modelManager->LoadModel("Missile.gltf", "Missile"); // ミサイル発射台


	modelManager->LoadModel("trainBridge.gltf", "stage/Bridge"); // ステージ(橋)
	//modelManager->LoadModel("trainBridge.obj", "stage/Bridge"); // ステージ(橋)
	modelManager->LoadModel("field.obj", "stage/Field"); // フィールド()
	modelManager->LoadModel("hasira.obj", "stage/objects"); // 柱()


	/// <summary>
	/// 敵
	/// </summary>

	modelManager->LoadModel("enemy.obj", "enemyAll/enemy"); // 的
	modelManager->LoadModel("enemy.gltf", "enemyAll/enemy2"); // 本体
	modelManager->LoadModel("enemy2.obj", "enemyAll/enemy2"); // 本体
	modelManager->LoadModel("enemyTire.obj", "enemyAll/tire"); // タイヤ
	modelManager->LoadModel("enemyDuct.obj", "enemyAll/duct"); // ダクト
	modelManager->LoadModel("enemyPlank.obj", "enemyAll/plank"); // 鋼板
	modelManager->LoadModel("enemyGear.obj", "enemyAll/gear"); // 歯車
	modelManager->LoadModel("enemyFence.obj", "enemyAll/fence"); // 柵


	modelManager->LoadModel("enemyBullet.gltf", "enemyAll/enemyBullet"); // 本体


}
