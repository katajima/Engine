#include "MyGame.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/application/scene/SceneFactory.h"

const float Engine::MyGame::kDeltaTime_ = 1.0f / 60.0f;
float Engine::MyGame::kTimeSpeed_ = 1.0f;
float Engine::MyGame::nowTime = 0.0f;

void Engine::MyGame::SetTimeScale(float timeScale) {
	// スロー演出などから呼ばれても破綻しないよう、時間倍率を安全な範囲に丸める
	if (timeScale < 0.0f) {
		timeScale = 0.0f;
	}
	if (timeScale > 4.0f) {
		timeScale = 4.0f;
	}

	kTimeSpeed_ = timeScale;
}

void Engine::MyGame::Initialize() {
	debugTimer_.StartTimer();

	Framework::Initialize();


	// 最初のシーン
	sceneFactory_ = std::make_unique<SceneFactory>();
	// シーンマネージャー
	sceneManager_ = std::make_unique<SceneManager>();

	// シーンマネージャーに最初のシーンをセット
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->SetInput(input_.get());
	// 各シーンから共通の音源一覧と再生機能を参照できるよう設定する。
	sceneManager_->SetAudioManager(audioManager_.get());
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

	// Resources/Sound以下の全WAVファイルを取得し、音源一覧と再生データを構築する。
	audioManager_->ReloadSoundFiles();
		
	// ModelData;
	LoadModel();


	/// <summary>
	/// パーティクル
	/// </summary>
	CreateParticle();
	

}

void Engine::MyGame::CreateParticle(){}

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
