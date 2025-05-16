#include "MyGame.h"

#include "DirectXGame/engine/Camera/Camera.h"

const float MyGame::kDeltaTime_ = 1.0f / 60.0f;
float MyGame::kTimeSpeed_ = 1.0f;
float MyGame::hitStopTimer = 0.0f;

void MyGame::Initialize()
{
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
	sceneManager_->ChangeScene("TEST");
	//sceneManager_->ChangeScene("GAMEPLAY");

	// リソース初期化
	InitializeResource();

	// グローバル変数の読み込み
	globalVariables_->LoadFiles();

}

void MyGame::Finalize()
{
	//
	//Audio::GetInstance()->Finalize();
	//


	// 基底クラスの終了処理
	Framework::Finalize();
}

void MyGame::Update()
{
	entity3DManager_->Get3DLineCommon()->LineClear();
	// ImGuiの受付開始
	dxCommon->GetImGuiManager()->Begin();

	Framework::Update();

	//HitStpoTime(); // ストップ用

#ifdef _DEBUG


	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();

	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}
	lastTime = currentTime;

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
	ImGui::Begin("GameScene");
	float width = static_cast<float> (WinApp::GetClientWidth() / 1.5f);
	float height = static_cast<float> (WinApp::GetClientHeight() / 1.5f);

	ImTextureID imguiTexture = (ImTextureID)(dxCommon->GetPostEffectManager()->GetEndRenderTexture()->GetSRVGPUHandle().ptr);
	ImGui::Image(imguiTexture, ImVec2(width, height));
	ImGui::End();
#endif // _DEBUG
	// ImGuiの受付終了
	dxCommon->GetImGuiManager()->End();
}

void MyGame::Draw()
{
	dxCommon->Draw(sceneManager_.get(), entity3DManager_.get());
}



void MyGame::InitializeResource()
{
	TextureManager* textureManager = dxCommon->GetTextureManager();
	ModelManager* modelManager = dxCommon->GetModelManager();
	ParticleManager* particleManager = entity3DManager_->GetEffectManager()->GetParticleManager();

	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/grass.png");
	textureManager->LoadTexture("resources/Texture/ground.png");
	textureManager->LoadTexture("resources/Texture/renga.png");
	textureManager->LoadTexture("resources/Texture/enemy.png");



	textureManager->LoadTexture("resources/Texture/aa.png");
	textureManager->LoadTexture("resources/Texture/aa2.png");
	textureManager->LoadTexture("resources/Texture/aaa.png");
	textureManager->LoadTexture("resources/Texture/aaaa.png");
	textureManager->LoadTexture("resources/Texture/aaaaa.png");
	textureManager->LoadTexture("resources/Texture/aaaaaa.png");
	textureManager->LoadTexture("resources/Texture/aa3.png");
	for (int i = 0; i < 10; i++) {
		std::string label = "resources/Texture/num/" + std::to_string(i) + ".png";
		textureManager->LoadTexture(label);
	}
	textureManager->LoadTexture("resources/Texture/num/Number_x64y96.png");
	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/dust.png");
	textureManager->LoadTexture("resources/Texture/hit.png");
	textureManager->LoadTexture("resources/Texture/text/max.png");
	textureManager->LoadTexture("resources/Texture/enemy.png");
	textureManager->LoadTexture("resources/Texture/circle2.png");
	textureManager->LoadTexture("resources/Texture/gradationLine.png");

	// 煙
	textureManager->LoadTexture("resources/Texture/smoke/no1.png");
	textureManager->LoadTexture("resources/Texture/smoke/no2.png");
	textureManager->LoadTexture("resources/Texture/smoke/no3.png");
	
	
	modelManager->LoadModel("d.gltf", "glTF");

	modelManager->LoadModel("a.obj");

	modelManager->LoadModel("multiMaterial.obj", "multiMaterial");

	modelManager->LoadModel("multiMaterial.gltf", "multiMaterial");

	modelManager->LoadModel("multiMesh.obj", "multiMesh");

	




	modelManager->LoadModel("walk.gltf", "human");
	modelManager->LoadModel("iku.gltf", "iku");



	modelManager->LoadModel("player_bullet.obj", "player_bullet");
	modelManager->LoadModel("Sword.obj", "Sword");
	modelManager->LoadModel("plane.obj", "plane");
	//modelManager->LoadModel("sneakWalk.gltf", "human");
	modelManager->LoadModel("AnimatedCube.gltf", "AnimatedCube");

	modelManager->LoadModel("Ground.obj", "Ground");
	modelManager->LoadModel("stair.obj");
	modelManager->LoadModel("BoxBox.obj", "BoxAABB");



	modelManager->LoadModel("plane.obj", "plane");
	modelManager->LoadModel("axis.obj", "axis");
	

	modelManager->LoadModel("teapot.obj", "teapot");



	modelManager->LoadModel("rail.obj", "rail");
	modelManager->LoadModel("Sphere.obj", "sphere");
	modelManager->LoadModel("Sphere2.obj", "sphere");
	modelManager->LoadModel("Sphere3.obj", "sphere");
	modelManager->LoadModel("skydome.obj", "skydome");



	

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



	/// <summary>
	/// 敵
	/// </summary>

	modelManager->LoadModel("enemy.obj", "enemyAll/enemy"); // 的
	modelManager->LoadModel("enemy2.obj", "enemyAll/enemy2"); // 本体
	modelManager->LoadModel("enemyTire.obj", "enemyAll/tire"); // タイヤ
	modelManager->LoadModel("enemyDuct.obj", "enemyAll/duct"); // ダクト
	modelManager->LoadModel("enemyPlank.obj", "enemyAll/plank"); // 鋼板
	modelManager->LoadModel("enemyGear.obj", "enemyAll/gear"); // 歯車
	modelManager->LoadModel("enemyFence.obj", "enemyAll/fence"); // 柵


	/// <summary>
	/// パーティクル
	/// </summary>

	primi = std::make_unique<Primitive>();
	ShapeParameter::Torus t;
	primi->Initialize<ShapeParameter::Torus>(entity3DManager_->GetPrimitiveCommon(),
		Primitive::ShapeType::Torus,
		t,
		"resources/Texture/uvChecker.png");

	primiTrai = std::make_unique<Primitive>();
	ShapeParameter::ShapeTriangle t2;
	primiTrai->Initialize<ShapeParameter::ShapeTriangle>(entity3DManager_->GetPrimitiveCommon(), Primitive::ShapeType::Triangle, t2, "resources/Texture/Image.png");

	primiPlane = std::make_unique<Primitive>();
	ShapeParameter::ShapePlane shapePlane;
	primiPlane->Initialize<ShapeParameter::ShapePlane>(entity3DManager_->GetPrimitiveCommon(), Primitive::ShapeType::Plane, shapePlane, "resources/Texture/uvChecker.png");


	ShapeParameter::Star star;
	star.innerRadius = 1.0f;
	star.outerRadius = 7.0f;
	star.segments = 4;
	primiStar = std::make_unique<Primitive>();
	primiStar->Initialize<ShapeParameter::Star>(entity3DManager_->GetPrimitiveCommon(), Primitive::ShapeType::Star, star, "resources/Texture/Image.png");

	particleManager->CreateParticleGroup("test", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));

	particleManager->CreateParticleGroup("cc", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"), {}, ParticleManager::BlendType::MODE_ADD);

	particleManager->CreateParticleGroup("dustt", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));


	particleManager->CreateParticleGroup("primi", "resources/Texture/uvChecker.png", primi.get());

	particleManager->CreateParticleGroup("primiPlane", "resources/Texture/circle2.png", primiPlane.get());


	particleManager->CreateParticleGroup("smoke", "resources/Texture/aa.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("explosionSmoke", "resources/Texture/aa.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);

	particleManager->CreateParticleGroup("dashEmit", "resources/Texture/aa.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("moveLimit", "resources/Texture/Image.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);

	// 煙
	particleManager->CreateParticleGroup("smokePlane01", "resources/Texture/smoke/no1.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane02", "resources/Texture/smoke/no2.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane03", "resources/Texture/smoke/no3.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane04", "resources/Texture/smoke/no4.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane01_1", "resources/Texture/smoke/no1.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane02_1", "resources/Texture/smoke/no2.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane03_1", "resources/Texture/smoke/no3.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane01_2", "resources/Texture/smoke/no1.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane02_2", "resources/Texture/smoke/no2.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("smokePlane03_2", "resources/Texture/smoke/no3.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);

	//
	particleManager->CreateParticleGroup("hitEffect", "resources/Texture/effect.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("hitEffect2", "resources/Texture/effect2.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	particleManager->CreateParticleGroup("hitEffect3", "resources/Texture/effect3.png", primiPlane.get()/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);


	// 敵関係
	particleManager->CreateParticleGroup("hitStar", "resources/Texture/Image.png", primiStar.get());
	particleManager->CreateParticleGroup("hitTrai", "resources/Texture/Image.png", primiTrai.get());
	particleManager->CreateParticleGroup("hit", "resources/Texture/hit.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyGround", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));
	particleManager->CreateParticleGroup("enemyDust", "resources/Texture/dust.png", modelManager->FindModel("plane.obj"));

	// 敵部品
	particleManager->CreateParticleGroup("enemyTire", "resources/Texture/enemy.png", modelManager->FindModel("enemyTire.obj")); // タイヤ
	particleManager->CreateParticleGroup("enemyDuct", "resources/Texture/enemy.png", modelManager->FindModel("enemyDuct.obj")); // ダクト
	particleManager->CreateParticleGroup("enemyPlank", "resources/Texture/enemy.png", modelManager->FindModel("enemyPlank.obj")); // 板
	particleManager->CreateParticleGroup("enemyGear", "resources/Texture/enemy.png", modelManager->FindModel("enemyGear.obj")); // 歯車
	particleManager->CreateParticleGroup("enemyFence", "resources/Texture/enemy.png", modelManager->FindModel("enemyFence.obj")); // 柵

	particleManager->CreateParticleGroup("dust", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));

}





void MyGame::HitStpoTime()
{
	bool is = false;
	hitStopTimer -= kDeltaTime_;
	if (hitStopTimer <= 0.0f) {
		hitStopTimer = 0.0f;
	}
	if (hitStopTimer > 0) {
		is = true;
	}

	if (is) {

		kTimeSpeed_ = 0.4f;
	}
	else {
		kTimeSpeed_ = 1.0f;
	}
}
