#include "MyGame.h"

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
	sceneManager_->SetDirectXCommon(dxCommon.get());
	sceneManager_->SetEntity3DManager(entity3DManager_.get());
	sceneManager_->SetEntity2DManager(entity2DManager_.get());

	sceneManager_->ChangeScene("TEST");
	//SceneManager::GetInstance()->ChangeScene("GAMEPLAY");

	// リソース初期化
	InitializeResource();

	// グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

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
	lastTime = currentTime;

	if (deltaTime > 0) {
		fps = 1.0f / deltaTime;
	}

	// FPS表示用ウィジェット
	ImGui::Begin("engine");
	ImGui::Text("FPS: %.2f", fps);
	ImGui::DragFloat("TimeScale", &kTimeSpeed_, 0.01f);
	ImGui::End();

#endif // _DEBUG

	// グローバル変数の更新
	GlobalVariables::GetInstance()->Update();

	// ライト
	entity3DManager_->GetLightManager()->Update(); 

	sceneManager_->Update();

	ParticleManager::GetInstance()->Update();

#ifdef _DEBUG
	entity3DManager_->Get3DLineCommon()->Update();
#endif // _DEBUG

	// ImGuiの受付終了
	dxCommon->GetImGuiManager()->End();
}

void MyGame::Draw()
{
	///
	// 描画前処理
	dxCommon->GetSrvManager()->PreDraw();
	// バックバッファの準備
	dxCommon->PreDrawSwap();
	

	////// オフスクリーン描画
	//dxCommon->PreDrawOffscreen(); // オフスクリーンのRTV設定
	
	
	

	

	// 3Dオブジェクトの描画
	sceneManager_->Draw3D();
	
	// パーティクル描画
	ParticleManager::GetInstance()->Draw();

#ifdef _DEBUG
	if (!sceneManager_->IsNowScene("GAMEPLAY")) {
		// デバッグ用のライン描画
		entity3DManager_->Get3DLineCommon()->Draw();
	}
#endif // _DEBUG

	// 2Dオブジェクトの描画
	sceneManager_->Draw2D();

	dxCommon->GetRenderTexture()->Draw();

	// ImGuiの描画
	dxCommon->GetImGuiManager()->Draw();

	// 描画後処理
	dxCommon->PostDrawSwap();
}


void MyGame::InitializeResource()
{
	TextureManager* textureManager = dxCommon->GetTextureManager();
	ModelManager* modelManager = dxCommon->GetModelManager();


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
	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/uvChecker.png");
	textureManager->LoadTexture("resources/Texture/Image.png");
	textureManager->LoadTexture("resources/Texture/dust.png");
	textureManager->LoadTexture("resources/Texture/hit.png");
	textureManager->LoadTexture("resources/Texture/text/max.png");
	textureManager->LoadTexture("resources/Texture/rostock_laage_airport_4k.dds");
	textureManager->LoadTexture("resources/Texture/enemy.png");
	textureManager->LoadTexture("resources/Texture/circle2.png");


	modelManager->LoadModelAmime("multiMaterial.gltf", "multiMaterial");

	modelManager->LoadModelAmime("multiMaterial.obj", "multiMaterial");
	modelManager->LoadModelAmime("a.obj");


	modelManager->LoadModelAmime("walk.gltf", "human");
	modelManager->LoadModelAmime("iku.gltf", "iku");
	


	modelManager->LoadModelAmime("player_bullet.obj", "player_bullet");
	modelManager->LoadModelAmime("Sword.obj", "Sword");
	modelManager->LoadModelAmime("plane.obj", "plane");
	//ModelManager::GetInstance()->LoadModelAmime("sneakWalk.gltf", "human");
	modelManager->LoadModelAmime("AnimatedCube.gltf", "AnimatedCube");

	modelManager->LoadModel("Ground.obj", "Ground");
	modelManager->LoadModel("stair.obj");
	modelManager->LoadModel("BoxBox.obj", "BoxAABB");



	modelManager->LoadModel("plane.obj", "plane");
	modelManager->LoadModel("axis.obj", "axis");
	modelManager->LoadModel("multiMesh.obj", "multiMesh");


	modelManager->LoadModel("multiMaterial.obj", "multiMaterial");
	modelManager->LoadModel("teapot.obj", "teapot");



	modelManager->LoadModel("rail.obj", "rail");
	modelManager->LoadModel("Sphere.obj", "sphere");
	modelManager->LoadModel("Sphere2.obj", "sphere");
	modelManager->LoadModel("Sphere3.obj", "sphere");
	modelManager->LoadModel("skydome.obj", "skydome");



	/// <summary>
	/// 地形
	/// </summary>
	modelManager->LoadModel("renga.gltf", "renga", { 10,10 });
	modelManager->LoadModel("coast.gltf", "terrain/coast", { 10,10 });
	modelManager->LoadModel("black.obj", "terrain/black", { 10,10 });
	modelManager->LoadModelAmime("stair.obj");



	/// <summary>
	/// 建物
	/// </summary>
	modelManager->LoadModelAmime("building.obj", "buildingAll/building"); // ビル
	modelManager->LoadModelAmime("warehouse.gltf", "buildingAll/warehouse"); // 倉庫



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
	primi->Initialize(entity3DManager_->GetPrimitiveCommon(), Primitive::ShapeType::Torus, "resources/Texture/uvChecker.png");

	primiTrai = std::make_unique<Primitive>();
	primiTrai->Initialize(entity3DManager_->GetPrimitiveCommon(),Primitive::ShapeType::Triangle, "resources/Texture/Image.png");

	primiPlane = std::make_unique<Primitive>();
	primiPlane->Initialize(entity3DManager_->GetPrimitiveCommon(),Primitive::ShapeType::Plane, "resources/Texture/uvChecker.png");

	primiStar = std::make_unique<Primitive>();
	primiStar->Initialize(entity3DManager_->GetPrimitiveCommon(),Primitive::ShapeType::Star, "resources/Texture/Image.png");
	ShapeParameter::Star star;
	star.innerRadius_ = 1.0f;
	star.outerRadius_ = 7.0f;
	star.segments_ = 4;
	primiStar->SetStar(star);
	primiStar->SetName("star");
	primiStar->Update();
	ParticleManager::GetInstance()->CreateParticleGroup("test", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));

	ParticleManager::GetInstance()->CreateParticleGroup("cc", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"), {}, ParticleManager::BlendType::MODE_ADD);

	ParticleManager::GetInstance()->CreateParticleGroup("dustt", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));


	ParticleManager::GetInstance()->CreateParticleGroup("primi", "resources/Texture/uvChecker.png", primi.get());

	ParticleManager::GetInstance()->CreateParticleGroup("primiPlane", "resources/Texture/circle2.png", primiPlane.get());


	ParticleManager::GetInstance()->CreateParticleGroup("smoke", "resources/Texture/aa.png", modelManager->FindModel("plane.obj"));
	ParticleManager::GetInstance()->CreateParticleGroup("explosionSmoke", "resources/Texture/aa.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);

	ParticleManager::GetInstance()->CreateParticleGroup("dashEmit", "resources/Texture/aa.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);
	ParticleManager::GetInstance()->CreateParticleGroup("moveLimit", "resources/Texture/Image.png", modelManager->FindModel("plane.obj")/*, {}, ParticleManager::BlendType::MODE_MUlLIPLY*/);

	// 敵関係
	ParticleManager::GetInstance()->CreateParticleGroup("hitStar", "resources/Texture/Image.png", primiStar.get());
	ParticleManager::GetInstance()->CreateParticleGroup("hitTrai", "resources/Texture/Image.png", primiTrai.get());
	ParticleManager::GetInstance()->CreateParticleGroup("hit", "resources/Texture/hit.png", modelManager->FindModel("plane.obj"));
	ParticleManager::GetInstance()->CreateParticleGroup("enemyGround", "resources/Texture/Image.png", modelManager->FindModel("plane.obj"));
	ParticleManager::GetInstance()->CreateParticleGroup("enemyDust", "resources/Texture/dust.png", modelManager->FindModel("plane.obj"));

	// 敵部品
	ParticleManager::GetInstance()->CreateParticleGroup("enemyTire", "resources/Texture/enemy.png", modelManager->FindModel("enemyTire.obj")); // タイヤ
	ParticleManager::GetInstance()->CreateParticleGroup("enemyDuct", "resources/Texture/enemy.png", modelManager->FindModel("enemyDuct.obj")); // ダクト
	ParticleManager::GetInstance()->CreateParticleGroup("enemyPlank", "resources/Texture/enemy.png",modelManager->FindModel("enemyPlank.obj")); // 板
	ParticleManager::GetInstance()->CreateParticleGroup("enemyGear", "resources/Texture/enemy.png", modelManager->FindModel("enemyGear.obj")); // 歯車
	ParticleManager::GetInstance()->CreateParticleGroup("enemyFence", "resources/Texture/enemy.png",modelManager->FindModel("enemyFence.obj")); // 柵

	ParticleManager::GetInstance()->CreateParticleGroup("dust", "resources/Texture/uvChecker.png", modelManager->FindModel("plane.obj"));

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
