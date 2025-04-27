#include "GamePlayScene.h"
#include <iostream>//用いるヘッダファイルが変わります。
#include <corecrt_math_defines.h>
#include <algorithm>

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"

#pragma region Initialize

// 初期化
void GamePlayScene::Initialize()
{
	input_ = GetInput();

	// カメラ
	InitializeCamera();
	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());


	// プレイヤー
	player_ = std::make_unique<Player>();
	player_->Initialize(GetDxCommon(), GetEntity3DManager(),GetEntity2DManager(), Vector3(0, 2, -40), camera.get());
	
	// フォローカメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(GetEntity3DManager()->GetCameraCommon());
	followCamera_->SetTarget(&player_->GetObject3D());

	// プレイヤー
	player_->SetInput(input_);
	player_->SetCamera(camera.get());
	player_->SetFollowCamera(followCamera_.get());

	// 敵マネージャ
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(GetEntity3DManager(), GetEntity2DManager(), camera.get());
	enemyManager_->SetPlayer(player_.get());

	for (int i = 0; i < 10; i++) {

		Vector3 rand = Random::RandomVector3(-100,100);
		rand.y = 2;
		enemyManager_->GenerateEnemy(EnemyManager::EnemyType::kNormal, rand);
	}

	
	
	
	

	// ステージ
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(GetDxCommon(), GetEntity3DManager(), GetEntity2DManager(), &followCamera_->GetViewProjection());





	


	// 衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize(GetGlobalVariables());

	InitializeResources();

	//LoadLevelData();

	GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(camera.get());
}


// カメラ初期化
void GamePlayScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();
	
	camera->Initialize(GetEntity3DManager()->GetCameraCommon());
	//camera = Camera::GetInstance();
	camera->transform_.rotate = { 0.36f,0,0 };
	camera->transform_.translate = { 5,32.5f,-59.2f };

	flag = true;
#ifdef _DEBUG

	//flag = false;

#endif // _DEBUG]

	SetCamera(camera.get());
}

// 各オブジェクトやスプライトなどの初期化
void GamePlayScene::InitializeResources()
{
	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());
	ParticleManager* particleManager = GetEntity3DManager()->GetEffectManager()->GetParticleManager();


	gameUI->Initialize(GetEntity2DManager());
	gameUI->SetPlayer(player_.get());


	float xpos = 1050;
	

	xpos = { 950 };

	numpos[2] = { xpos,100 };
	numpos[1] = { xpos + (50 * 1),100 };
	numpos[0] = { xpos + (50 * 2),100 };




	for (int i = 0; i < 3; i++) {
		auto sprite = std::make_unique<Sprite>();

		sprite->Initialize(GetEntity2DManager()->GetSpriteCommon(), "resources/Texture/num/Number_x64y96.png", false);
		sprite->SetTextureSize({ 64,96 });
		sprite->SetSize({ 64 * 2 / 3, 96 * 2 /3 });

		sprite->SetPosition(numpos[i]);

		sprite_.push_back(std::move(sprite));
	}


	emit_ = std::make_unique<ParticleEmitter>();
	emit_->Initialize(particleManager,"groundRtttight", "dustt");
	emit_->GetFrequency() = 0.5f;
	emit_->SetCount(200);
	emit_->SetPos({ 200,40,200 });
	emit_->SetVelocityMinMax(-Vector3{0.2f,0.2f,0.2f }, { 0.2f, 0.2f, 0.2f });
	emit_->SetLifeTimeMinMax(10.4f, 10.7f);
	emit_->SetIsAlpha(true);
	emit_->SetSizeMinMax(Vector3{ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f });
	emit_->SetColorMinMax({ 0.604f, 0.384f, 0.161f }, { 0.604f, 0.384f, 0.161f });
	emit_->SetRengeMinMax({-400,-100,-400}, { 400,100,400 });











	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 2.0f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;


	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	GetEntity3DManager()->GetLightManager()->AddLight(directional);


}
// 
void GamePlayScene::LoadLevelData()
{
	const std::string kDefaultBaseDirectory = "resources/";
	const std::string fileName = "levelData/";
	const std::string kExtension = "untitled.json";
	// 凍結してフルパスを得る 
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;
	// ファイルストリーム 
	std::ifstream file; // ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗チェック 
	if (file.fail()) { assert(0); }
	// JSON文字列から解凍したデータ 
	nlohmann::json deserialized;
	// 解凍 
	file >> deserialized;
	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());
	// "name"を文字列として取得 
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック 
	assert(name.compare("scene") == 0);
	// レベルデータ格納用インスタンスを生成 
	LevelData* levelData = new LevelData();
	//std::map<std::string, std::unique_ptr <Model>> models;
	const auto& models = GetDxCommon()->GetModelManager()->GetModel();
	//models = ModelManager::GetInstance()->GetModel();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));
		// 種別を取得 
		std::string type = object["type"].get<std::string>();
		if (type.compare("MESH") == 0) {
			// 要素追加 
			levelData->objects.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る 
			LevelData::ObjectData& objectData = levelData->objects.back();
			if (object.contains("name")) {
				// ファイル名 
				objectData.fileName = object["name"];
			}
			// トランスフォームのパラメータ読み込み 
			nlohmann::json& transform = object["transform"];
			// 平行移動 
			objectData.position.x = (float)transform["translation"][0];
			objectData.position.y = (float)transform["translation"][1];
			objectData.position.z = (float)transform["translation"][2];
			// 回転 
			objectData.rotation.x = (float)transform["rotation"][0];
			objectData.rotation.y = (float)transform["rotation"][1];
			objectData.rotation.z = (float)transform["rotation"][2];
			// スケーリング 
			objectData.scale.x = (float)transform["scaling"][0];
			objectData.scale.y = (float)transform["scaling"][1];
			objectData.scale.z = (float)transform["scaling"][2];
			// 再帰関数にまとめ、再帰呼出で枝を走査する 
			if (object.contains("children")) {}
		} for (auto& objectData : levelData->objects) {
			// ファイル名から登録済みモデルを検索
			Model* model = nullptr;
			auto it = models.find(objectData.fileName + ".obj");
			if (it != models.end()) { model = it->second.get(); }
			// モデルを指定して3Dオブジェクトを生成 
			Object3d* newObject = new Object3d();
			//ModelManager::GetInstance()->LoadModel(objectData.fileName + ".obj");
			newObject->Initialize(GetEntity3DManager());
			newObject->SetModel(model);
			newObject->SetCamera(camera.get());
			// 座標 
			newObject->worldtransform_.translate_ = objectData.position;
			// 回転角 
			newObject->worldtransform_.rotate_ = objectData.rotation;
			// 大きさ
			newObject->worldtransform_.scale_ = objectData.scale;
			// 配列に登録
			objects.push_back(newObject);
		}
	}
}
// 調整項目
void GamePlayScene::ApplyGlobalVariables()
{

}

void GamePlayScene::CheckAllCollisions()
{
	// 衝突マネージャのリセット
	collisionManager_->Reset();
	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());
	if (player_->GetBehavior() == Player::Behavior::kAttack) {
		// コライダーをリストに登録
		collisionManager_->AddCollider(player_->GetWeapon());
	}

	for (const auto& bullet : player_->GetBullets()) {
		collisionManager_->AddCollider(bullet.get());
	}

	for (auto enemy : enemyManager_->GetEnemys()) {
		collisionManager_->AddCollider(enemy);
	}

	// 衝突判定
	collisionManager_->CheckAllCollisions();
}
#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{

#ifdef _DEBUG
	if (input_->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		GetSceneManager()->ChangeScene("TITLE");
	}
	Vector2 pos = player_->GetObject3D().GetScreenPosition();
	ImGui::Begin("engine");
	ImGui::Checkbox("flag", &flag);
	ImGui::DragFloat2("screenpos", &pos.x, 0.1f);
	ImGui::End();

	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("Translate", &camera->transform_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &camera->transform_.rotate.x, 0.01f);
		ImGui::Checkbox("flag", &flag);
		if (ImGui::Button("cameraPos")) {
			camera->transform_.translate = { 0,20,-175 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos2")) {
			camera->transform_.translate = { -30,10,-140 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos3")) {
			camera->transform_.translate = { 0,500,0 };
			camera->transform_.rotate = { DegreesToRadians(90),0,0 };
		}
		if (ImGui::Button("cameraPos4")) {
			camera->transform_.translate = { 0,60,-220 };
			camera->transform_.rotate = { DegreesToRadians(10),0,0 };
		}


	}

	if (ImGui::TreeNode("Test")) {
		ImGui::Text("Camera1");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		//ImGui::TreePop();
		ImGui::Text("Camera2");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		ImGui::TreePop();
	}


	ImGui::End();

#endif
}

// 更新処理
void GamePlayScene::Update()
{
	Camera::isShake_ = false;

	// 調整項目
	ApplyGlobalVariables();

	// ImGuiの更新
	UpdateImGui();



	if (behaviorRequest_) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_) {
		case Behavior::kPhase1: // フェーズ１
			BehaviorPhase1Initialize();
			break;
		case Behavior::kPhase2: // フェーズ２
			BehaviorPhase2Initialize();
			break;
		}
		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}
	switch (behavior_) {
	case Behavior::kPhase1: // フェーズ１
		BehaviorPhase1Update();
		break;
	case Behavior::kPhase2: // フェーズ２
		BehaviorPhase2Update();
		break;
	}

	// プレイヤー
	//if (player_->GetAlive()) {
		player_->Update();
		player_->LockOn(enemyManager_->GetEnemys());
	//}
	
		

	/// レールカメラ
	// カメラの回転を設定
	if (flag) {

		followCamera_->Update();
		camera->viewMatrix_ = followCamera_->GetViewProjection().viewMatrix_;
		camera->projectionMatrix_ = followCamera_->GetViewProjection().projectionMatrix_;


		GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(&followCamera_->GetViewProjection());
		
		// 必要に応じて行列を更新
		//camera->UpdateMatrix();
	}
	else {
#ifdef _DEBUG
#endif // _DEBUG

		GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());
		camera->UpdateMatrix();
	}





	



	
	

	// デバック表示用にワールドトランスフォームを更新
	collisionManager_->UpdateWorldTransform();


	//player_->SetCamera(camera.get());
	
	// ステージ
	stage_->Update();
	// 当たり判定
	CheckAllCollisions();
}

#pragma endregion //更新関係

#pragma region BehaviorPhase

void GamePlayScene::BehaviorPhase1Initialize()
{
}

void GamePlayScene::BehaviorPhase1Update()
{

	enemyManager_->Update();

}

void GamePlayScene::BehaviorPhase2Initialize()
{
}

void GamePlayScene::BehaviorPhase2Update()
{
}
#pragma endregion // フェーズ


#pragma region 


#pragma endregion その他

// 終了
void GamePlayScene::Finalize()
{
	for (auto& object : objects) {
		delete object;
	}
}

// 3D描画
void GamePlayScene::Draw3D()
{
	////3Dオブジェクトの描画

	// ステージ
	stage_->Draw();

	// プレイヤー
	player_->Draw();

	
	enemyManager_->Draw();

	// パーティクル
	player_->DrawP();

	
	
	
	// パーティクル	
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->Draw();
	
	stage_->DrawP();

	// 当たり判定の表示
	collisionManager_->Draw();

}

// 2D描画
void GamePlayScene::Draw2D()
{


	//////////////--------スプライト-----------///////////////////
	int adsbhads = player_->GetHitCount();
	if (adsbhads >= 999) {
		adsbhads = 999;
	}

	int numDigits = (adsbhads == 0) ? 1 : static_cast<int>(log10(adsbhads)) + 1;


	Vector2 texSize = { 64, 96 };

	for (int i = 0; i < sprite_.size(); i++) {
		int digit = (static_cast<int>(adsbhads) / static_cast<int>(pow(10, i))) % 10;
		sprite_[i]->SetTextureLeftTop(Vector2((texSize.x * digit), 0));


		sprite_[i]->Update();
		sprite_[i]->Draw();
	}


	// ゲームUI
	gameUI->Draw();
	
	// 敵スプライト
	enemyManager_->Draw2D();

	// プレイヤースプライト
	player_->Draw2D();


	




	//if (!player_->GetAlive()) {
	//	sceneCount++;
	//	if (clock == 1) {
	//		//text_over->Update();
	//		//text_over->Draw();
	//	}
	//}
	//else if (count >= enemys_.size()) {
	//	sceneCount++;
	//	if (clock == 1) {
	//		//text_clera->Update();
	//		//text_clera->Draw();
	//	}
	//}
	//if (sceneCount % 15 == 0) {
	//	clock *= -1;
	//}

	//if (sceneCount >= 240) {
	//	//GetSceneManager()->ChangeScene("TITLE");
	//}




}

