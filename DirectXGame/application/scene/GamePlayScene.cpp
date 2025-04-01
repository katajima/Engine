#include "GamePlayScene.h"
#include <iostream>//用いるヘッダファイルが変わります。
#include <corecrt_math_defines.h>
#include <algorithm>

#pragma region Initialize

// 初期化
void GamePlayScene::Initialize()
{
	// カメラ
	InitializeCamera();
	// オブジェクト3D
	Object3dCommon::GetInstance()->SetDefaltCamera(camera.get());


	//followCamera_ = std::make_unique<FollowCamera>();
	//followCamera_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize(Vector3(0, 2, -40), camera.get());



	//followCamera_->SetTarget(&player_->GetObject3D());

	//player_->SetCamera(&followCamera_->GetViewProjection());
	//player_->SetFollowCamera(followCamera_.get());
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();




	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize({ 10000,10000 });
	ocean_->SetCamera(camera.get());
	ocean_->transform.rotate.x = DegreesToRadians(90);
	ocean_->transform.translate.y = -10;
	ocean_->material->color = { 0,0,0.57f,1 };
	ocean_->material->color.a = 0.95f;


	tail = std::make_unique<Object3d>();
	tail->Initialize();
	tail->SetModel("renga.gltf");
	tail->SetCamera(camera.get());
	tail->worldtransform_.scale_ = { 10,10,10 };

	tail2 = std::make_unique<Object3d>();
	tail2->Initialize();
	tail2->SetModel("black.obj");
	tail2->SetCamera(camera.get());
	tail2->worldtransform_.scale_ = { 104,104,104 };
	tail2->worldtransform_.translate_.y = -20;

	sky.Initialize();
	sky.SetModel("skydome.obj");
	sky.SetCamera(camera.get());
	sky.worldtransform_.scale_ = { 100,100,100 };
	sky.model->modelData.material[0]->enableLighting_ = false;


	warePos.push_back({ 300,0,-200 });
	warePos.push_back({ 300,0,-100 });
	warePos.push_back({ 300,0,0 });
	warePos.push_back({ 300,0,100 });
	warePos.push_back({ 300,0,200 });

	for (int i = 0; i < warePos.size(); i++) {
		auto obj = std::make_unique<Object3d>();
		obj->Initialize();
		obj->SetModel("warehouse.gltf");
		obj->SetCamera(camera.get());
		obj->worldtransform_.scale_ = { 2, 2, 2 };
		obj->worldtransform_.translate_ = warePos[i];
		obj->worldtransform_.rotate_.y = DegreesToRadians(90);


		warehouseObject.push_back(std::move(obj));
	}

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(camera.get(), nullptr, player_.get());

	//enemyManager_->AddMoveTarget(100, { 10,2,10 });


	// 衝突マネージャの生成
	collisionManager_ = std::make_unique<CollisionManager>();
	collisionManager_->Initialize();

	InitializeResources();



	LoadLevelData();

	LineCommon::GetInstance()->SetDefaltCamera(camera.get());
}


// カメラ初期化
void GamePlayScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();
	camera->Initialize();
	//camera = Camera::GetInstance();
	camera->transform_.rotate = { DegreesToRadians(-270),0,0};
	camera->transform_.translate = { 0,132.5f,0 };

	cameraDebugT = camera->transform_.translate;
	cameraDebugR = camera->transform_.rotate;

	cameraT.y = 1.0f;


	//flag = true;
#ifdef _DEBUG

	//flag = false;

#endif // _DEBUG]


	cameraObj_.Initialize();
}

// 各オブジェクトやスプライトなどの初期化
void GamePlayScene::InitializeResources()
{
	// オブジェクト3D
	Object3dCommon::GetInstance()->SetDefaltCamera(camera.get());





	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* gropName = "sprite";

	// グループを追加する 
	GlobalVariables::GetInstance()->CreateGroup(gropName);



	emit_ = std::make_unique<ParticleEmitter>();
	emit_->Initialize("groundRtttight", "dustt");
	emit_->GetFrequency() = 0.5f;
	emit_->SetCount(200);
	emit_->SetPos({ 200,40,200 });
	emit_->SetVelocityMinMax(-Vector3{ 0.2f,0.2f,0.2f }, { 0.2f, 0.2f, 0.2f });
	emit_->SetLifeTimeMinMax(10.4f, 10.7f);
	emit_->SetIsAlpha(true);
	emit_->SetSizeMinMax(Vector3{ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f });
	emit_->SetColorMinMax({ 0.604f, 0.384f, 0.161f }, { 0.604f, 0.384f, 0.161f });
	emit_->SetRengeMinMax({ -400,-100,-400 }, { 400,100,400 });




	moveLimitEmitter_ = std::make_unique <ParticleEmitter>();
	moveLimitEmitter_->Initialize("dash", "moveLimit", ParticleEmitter::EmitSpawnShapeType::kCornerLine);
	moveLimitEmitter_->GetFrequency() = 0.5f;
	moveLimitEmitter_->SetCount(100);
	moveLimitEmitter_->SetLifeTimeMinMax(0.5f, 0.5f);
	//moveLimitEmitter_->SetIsAlpha(true);
	//moveLimitEmitter_->SetIsEmit(false);
	moveLimitEmitter_->SetColorMinMax({ 0.7f,0.7f,0.7f,0.9f }, { 0.7f,0.7f,0.7f,0.9f });
	moveLimitEmitter_->SetRengeMinMax({ -1.25f,-1.25f ,-1.25f }, { 1.25f,1.25f,1.25f });
	moveLimitEmitter_->SetSizeMinMax(Vector3{ 1.1f,1.1f,1.1f }, { 1.1f,1.1f,1.1f });
	moveLimitEmitter_->SetVelocityMinMax({}, {});
	moveLimitEmitter_->SetPos({ 0,10,0 });
	moveLimitEmitter_->SetCorner(4, 300);
	moveLimitEmitter_->transform_.rotate_.y = DegreesToRadians(45);










	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 2.0f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;


	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	LightManager::GetInstance()->AddLight(directional);


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
	const auto& models = ModelManager::GetInstance()->GetModel();
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
			newObject->Initialize();
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
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* gropName = "sprite";


	// グループを追加する 
	GlobalVariables::GetInstance()->CreateGroup(gropName);
	for (int j = 0; j < 3; j++) {
		std::string str = "posNum" + std::to_string(j);

		//numpos[j] = globalVariables->GetVector2Value(gropName, str);

	}



}

void GamePlayScene::CheckAllCollisions()
{
	// 衝突マネージャのリセット
	collisionManager_->Reset();
	// コライダーをリストに登録
	collisionManager_->AddCollider(player_.get());






	// 衝突判定
	collisionManager_->CheckAllCollisions();
}
#pragma endregion 初期化関係


#pragma region UpDate

// ImGui更新
void GamePlayScene::UpdateImGui()
{

#ifdef _DEBUG
	if (Input::GetInstance()->IsTriggerKey(DIK_P)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	Vector2 pos = player_->GetObject3D().GetScreenPosition();
	ImGui::Begin("engine");
	ImGui::Checkbox("flag", &flag);
	ImGui::DragFloat2("screenpos", &pos.x, 0.1f);
	ImGui::End();

	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("followCamera")) {
		//ImGui::DragFloat3("Rotate", &followCamera_->GetViewProjection().transform_.rotate.x,0.01f);
		//ImGui::DragFloat3("translate", &followCamera_->GetViewProjection().transform_.translate.x);
		//Vector3 degree = RadiansToDegrees(followCamera_->GetViewProjection().transform_.rotate);
		//ImGui::InputFloat3("Degree", &degree.x);
		//Vector3 forward = followCamera_->GetViewProjection().GetForward();
		//ImGui::InputFloat3("forward", &forward.x);


		
	}

	/*if (ImGui::CollapsingHeader("Camera")) {
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


	}*/

	//if (ImGui::TreeNode("Test")) {
	//	ImGui::Text("Camera1");
	//	ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
	//	//ImGui::TreePop();
	//	ImGui::Text("Camera2");
	//	ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
	//	ImGui::TreePop();
	//}


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


	camera->transform_.translate = player_->GetObject3D().GetWorldPosition();
	camera->transform_.translate.y = 150;
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

	//}

	enemyManager_->Update();


	/// レールカメラ
	// カメラの回転を設定
	if (flag) {

		//followCamera_->Update();
		//camera->viewMatrix_ = followCamera_->GetViewProjection().viewMatrix_;
		//camera->projectionMatrix_ = followCamera_->GetViewProjection().projectionMatrix_;


		//ParticleManager::GetInstance()->SetCamera(&followCamera_->GetViewProjection());
		// 必要に応じて行列を更新
		//camera->UpdateMatrix();
	}
	else {
#ifdef _DEBUG
#endif // _DEBUG

		ParticleManager::GetInstance()->SetCamera(camera.get());
		camera->UpdateMatrix();
	}









	// タイル
	tail->Update();
	tail2->Update();
	sky.Update();
	for (int i = 0; i < warehouseObject.size(); i++) {
		//	warehouseObject[i]->Update();
	}

	ocean_->Update();

	//moveLimitEmitter_->Update();
	//emit_->Update();
	// デバック表示用にワールドトランスフォームを更新
	collisionManager_->UpdateWorldTransform();


	player_->SetCamera(camera.get());


	CheckAllCollisions();
}

#pragma endregion //更新関係

#pragma region BehaviorPhase

void GamePlayScene::BehaviorPhase1Initialize()
{
}

void GamePlayScene::BehaviorPhase1Update()
{

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
	//sky.Draw();



	tail->Draw();
	//tail2->Draw(); 
	for (int i = 0; i < warehouseObject.size(); i++) {
		//	warehouseObject[i]->Draw();
	}

	////3Dオブジェクトの描画


	player_->Draw();


	enemyManager_->Draw();


	// パーティクル
	player_->DrawP();


	ParticleManager::GetInstance()->GetInstance()->Draw();
	//ParticleManager::GetInstance()->GetInstance()->DrawAABB();


	//ocean_->Draw();


	// 当たり判定の表示
	collisionManager_->Draw();

}

// 2D描画
void GamePlayScene::Draw2D()
{

}

