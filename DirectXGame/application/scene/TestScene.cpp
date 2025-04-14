#include"TestScene.h"
//#include"DirectXGame/engine/struct/Quaternion.h"
#include "DirectXGame/engine/SkyBox/SkyBoxCommon.h"

#include "DirectXGame/engine/math/Random.h"

void TestScene::Initialize()
{
	//オーディオの初期化
	audio_ = Audio::GetInstance();
	// 入力初期化
	input_ = Input::GetInstance();
	// ImGui初期化
	imGuiManager = ImGuiManager::GetInstance();


	// カメラの初期化
	InitializeCamera();

	// 3Dオブジェクトの初期化
	InitializeObject3D();

	// 2Dオブジェクトの初期化
	InitializeObject2D();

	// パーティクルの初期化
	InitializeParticle();

	// ライトの初期化
	InitializeLight();

	// その他の初期化
	InitializeOthers();

}

void TestScene::Finalize()
{

}

void TestScene::Update()
{
	SwitchRoom(); // 部屋切り替え

#ifdef _DEBUG
	ImGui::Begin("primi2D");
	ImGui::DragFloat2("pos", &primitive2d1_->position.x);
	ImGui::DragFloat2("scale", &primitive2d1_->scale.x, 0.1f);
	ImGui::DragFloat("rotate", &primitive2d1_->rotation, 0.01f);
	ImGui::DragInt("segment", &segment);
	ImGui::DragFloat("inRad", &inRad);
	ImGui::DragFloat("outRad", &outRad);
	if (segment < 3) {
		segment = 3;
	}
	if (inRad >= outRad) {
		inRad = outRad;
	}
	primitive2d1_->SetParametar(inRad, outRad, segment);
	ImGui::End();

	ImGui::Begin("engine");
	ImGui::Checkbox("debugCamera", &isDebugCamera);




	if (isDebugCamera) {
		debugCamera->Update();

		camera->viewMatrix_ = debugCamera->GetViewProjection().viewMatrix_;
		camera->projectionMatrix_ = debugCamera->GetViewProjection().projectionMatrix_;
		camera->transform_ = debugCamera->GetViewProjection().transform_;
	}
	else {

	}


	ImGui::End();


#endif // _DEBUG

	camera->UpdateMatrix();


	if (behaviorRequest_) {

		// ふるまいを変更する
		behavior_ = behaviorRequest_.value();
		// 各ふるまいごとの初期化を実行
		switch (behavior_)
		{
		case TestScene::SceneBehavior::kSceneRoom01:
			InitializeRoom01();
			break;
		case TestScene::SceneBehavior::kSceneRoom02:
			InitializeRoom02();
			break;
		case TestScene::SceneBehavior::kSceneRoom03:
			InitializeRoom03();
			break;
		case TestScene::SceneBehavior::kSceneRoom04:
			InitializeRoom04();
			break;
		case TestScene::SceneBehavior::kSceneRoom05:
			InitializeRoom05();
			break;
		case TestScene::SceneBehavior::kSceneRoom06: // Octtree
			InitializeRoom06();
			break;
		case TestScene::SceneBehavior::kSceneRoom07: // 線形探索
			InitializeRoom07();
			break;
		case TestScene::SceneBehavior::kSceneRoom08: // ノイズ
			InitializeRoom08();
			break;
		case TestScene::SceneBehavior::kSceneRoom09:
			break;
		case TestScene::SceneBehavior::kSceneRoom10:
			break;
		default:
			break;
		}

		// ふるまいリクエストリセット
		behaviorRequest_ = std::nullopt;
	}


	switch (behavior_)
	{
	case TestScene::SceneBehavior::kSceneRoom01:
		UpdateRoom01();
		break;
	case TestScene::SceneBehavior::kSceneRoom02:
		UpdateRoom02();
		break;
	case TestScene::SceneBehavior::kSceneRoom03:
		UpdateRoom03();
		break;
	case TestScene::SceneBehavior::kSceneRoom04:
		UpdateRoom04();
		break;
	case TestScene::SceneBehavior::kSceneRoom05:
		UpdateRoom05();
		break;
	case TestScene::SceneBehavior::kSceneRoom06:
		UpdateRoom06();
		break;
	case TestScene::SceneBehavior::kSceneRoom07:
		UpdateRoom07();
		break;
	case TestScene::SceneBehavior::kSceneRoom08:
		UpdateRoom08();
		break;
	case TestScene::SceneBehavior::kSceneRoom09:
		break;
	case TestScene::SceneBehavior::kSceneRoom10:
		break;
	default:
		break;
	}
	tail.Update();
}

void TestScene::Draw3D()
{
	switch (behavior_)
	{
	case TestScene::SceneBehavior::kSceneRoom01:
		tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);
		ocean_->Draw();
		GetEntity3DManager()->GetSkyBoxCommon()->DrawCommonSetting();
		break;
	case TestScene::SceneBehavior::kSceneRoom02:
		tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case TestScene::SceneBehavior::kSceneRoom03:
		tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);

		multiy.Draw();
		break;
	case TestScene::SceneBehavior::kSceneRoom04:
		tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);

		skinningObject.DrawSkinning();
		skinningObject2.DrawSkinning();

		break;
	case TestScene::SceneBehavior::kSceneRoom05:
		//tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);
		break;
	case TestScene::SceneBehavior::kSceneRoom06:

		//stairObject->Draw();
		break;
	case TestScene::SceneBehavior::kSceneRoom07:

		// プレイヤー
		playerObject->Draw();

		// ゴール
		goalObject->Draw();

		//Object3dInstansManager::GetInstance()->Update();
		//Object3dInstansManager::GetInstance()->Draw();

		break;
	case TestScene::SceneBehavior::kSceneRoom08:

		GetEntity3DManager()->GetObject3dInstansManager()->Update();
		GetEntity3DManager()->GetObject3dInstansManager()->Draw();

		break;
	case TestScene::SceneBehavior::kSceneRoom09:
		break;
	case TestScene::SceneBehavior::kSceneRoom10:
		break;
	default:
		break;
	}



}

void TestScene::Draw2D()
{
	switch (behavior_)
	{
	case TestScene::SceneBehavior::kSceneRoom01:
		for (int i = 0; i < sprite_.size(); i++) {
			sprite_[i]->UpdateAmimetion(0.05f);
		}
		sprite_[0]->Draw();
		sprite_[1]->Draw(Sprite::SpriteType::NoUvInterpolation_MODE_SOLID);
		sprite_[2]->Draw(Sprite::SpriteType::UvInterpolation_MODE_WIREFRAME);
		sprite_[3]->Draw(Sprite::SpriteType::NoUvInterpolation_MODE_WIREFRAME);



		primitive2d1_->Update();
		primitive2d1_->Draw();



		break;
	case TestScene::SceneBehavior::kSceneRoom02:
		break;
	case TestScene::SceneBehavior::kSceneRoom03:
		break;
	case TestScene::SceneBehavior::kSceneRoom04:
		break;
	case TestScene::SceneBehavior::kSceneRoom05:
		break;
	case TestScene::SceneBehavior::kSceneRoom06:
		break;
	case TestScene::SceneBehavior::kSceneRoom07:
		break;
	case TestScene::SceneBehavior::kSceneRoom08:
		break;
	case TestScene::SceneBehavior::kSceneRoom09:
		break;
	case TestScene::SceneBehavior::kSceneRoom10:
		break;
	default:
		break;
	}


}


#pragma region Initialize

/// <summary>
/// 3Dオブジェクトの初期化
/// </summary>
void TestScene::InitializeObject3D()
{
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());

	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize(GetEntity3DManager(), { 100,100 });
	ocean_->SetCamera(camera.get());
	ocean_->transform.rotate.x = DegreesToRadians(90);
	ocean_->material->color.a = 0.99f;

	skinningObject.Initialize(GetEntity3DManager());
	skinningObject.SetModel("iku.gltf");
	skinningObject.worldtransform_.translate_ = { 30,1,1 };
	skinningObject.worldtransform_.scale_ = { 10,10,10 };
	skinningObject.SetCamera(camera.get());


	skinningObject2.Initialize(GetEntity3DManager());
	skinningObject2.SetModel("walk.gltf");
	skinningObject2.worldtransform_.translate_ = { -30,10,1 };
	skinningObject2.worldtransform_.scale_ = { 10,10,10 };
	skinningObject2.SetCamera(camera.get());


	tail.Initialize(GetEntity3DManager());
	tail.SetModel("renga.gltf");
	tail.SetCamera(camera.get());
	tail.model->modelData.material[0]->shininess_ = 1000.0f;

	multiy.Initialize(GetEntity3DManager());
	multiy.SetModel("multiMaterial.gltf");
	multiy.SetCamera(camera.get());
	multiy.worldtransform_.scale_ = { 10,10,10 };

	/// 階段
	stairObject = std::make_unique<Object3d>();
	stairObject->Initialize(GetEntity3DManager());
	stairObject->SetModel("stair.obj");
	stairObject->SetCamera(camera.get());


	tri2d.vertices[0] = { 10,0, };
	tri2d.vertices[1] = { 10,10, };
	tri2d.vertices[2] = { -10,0, };

	// プレイヤーオブジェクト
	playerObject = std::make_unique<Object3d>();
	playerObject->Initialize(GetEntity3DManager());
	playerObject->SetModel("teapot.obj");
	playerObject->SetCamera(camera.get());
	playerObject->worldtransform_.translate_ = { 10,10,10 };
	playerObject->worldtransform_.scale_ = 3;


	// ゴールのオブジェクト
	goalObject = std::make_unique<Object3d>();
	goalObject->Initialize(GetEntity3DManager());
	goalObject->SetModel("Sphere.obj");
	goalObject->SetCamera(camera.get());
	goalObject->worldtransform_.translate_ = { 200,10,200 };
	goalObject->worldtransform_.scale_ = 3;

	GetEntity3DManager()->GetObject3dInstansManager()->SetCamera(camera.get());


	for (int i = 0; i < map->GetWidth(); i++) {
		for (int j = 0; j < map->GetHeight(); j++) {
			ObjectInstans obj{};
			obj.Initialize();
			obj.transform.scale_ = { 5,5,5 };
			obj.transform.translate_.x = float(10 * i);
			obj.transform.translate_.z = float(10 * j);
			obj.transform.translate_.y = noise->PerlinNoise(float(i), float(j)) * obj.transform.scale_.y;





			GetEntity3DManager()->GetObject3dInstansManager()->AddObject("BoxBox.obj", "resources/Texture/renga.png", obj);

		}
	}


}

/// <summary>
/// スプライトの初期化
/// </summary>
void TestScene::InitializeObject2D()
{
	for (int i = 0; i < 4; i++)
	{
		auto sprite = std::make_unique<Sprite>();

		sprite->Initialize("resources/Texture/uvChecker.png", false);
		sprite->SetTextureSize({ 64,64 });
		sprite->SetSize({ 128,128 });
		sprite->SetAnimeSize({ 64,64 });
		sprite->SetMaxAnimeNum({ 8,8 });
		sprite->SetPosition({ static_cast<float>(i) * (128 + 10) ,0 });

		sprite_.push_back(std::move(sprite));
	}

	///
	primitive2d1_ = std::make_unique<Primitive2D>();
	primitive2d1_->Initialize(Primitive2D::ShapeType::Ring, { 1,1,1,1 });
	primitive2d1_->position = { 640,360 };
	//primitive2d1_->rotation = DegreesToRadians(45);

}

/// <summary>
/// パーティクルの初期化
/// </summary>
void TestScene::InitializeParticle()
{
	ParticleManager::GetInstance()->SetCamera(camera.get());


	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize("emitter", "cc", ParticleEmitter::EmitSpawnShapeType::kSpline);
	emitter_->GetFrequency() = 0.1f;
	emitter_->SetCount(1);
	emitter_->SetParent(tail.worldtransform_);
	emitter_->SetRotateMinMax(-Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	emitter_->SetPos({ 0,10,0 });
	emitter_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	emitter_->SetLifeTimeMinMax(1.0f, 2.0f);
	/*emitter_->SetUsebillboard(false);*/
	emitter_->SetIsGravity(true);
	emitter_->SetIsAlpha(true);
	emitter_->AddControlPoints(Vector3{ 0,0,0 });
	emitter_->AddControlPoints(Vector3{ 10,10,0 });
	emitter_->AddControlPoints(Vector3{ 20,20,0 });
	emitter_->AddControlPoints(Vector3{ 30,30,0 });





	emitterEnemy_ = std::make_unique<ParticleEmitter>();
	emitterEnemy_->Initialize("emitterPrimi", "primi");
	emitterEnemy_->GetFrequency() = 0.1f;
	emitterEnemy_->SetCount(1);
	//emitterEnemy_->SetParent(mm.worldtransform_);
	emitterEnemy_->SetPos({ 0,50,0 });
	emitterEnemy_->SetVelocityMinMax({ -0,20,-5 }, { 5, 20, 5 });
	emitterEnemy_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	emitterEnemy_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	emitterEnemy_->SetLifeTimeMinMax(5, 10);
	emitterEnemy_->SetIsGravity(true);
	emitterEnemy_->SetUsebillboard(false);
	emitterEnemy_->SetIsAlpha(true);
	emitterEnemy_->SetIsLifeTimeScale(true);
	emitterEnemy_->SetIsRotateVelocity(true);
	emitterEnemy_->SetIsBounce(true);
	emitterEnemy_->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f });

	primitvPlane_ = std::make_unique<ParticleEmitter>();
	primitvPlane_->Initialize("primiPlane", "primiPlane", ParticleEmitter::EmitSpawnShapeType::kPoint);
	primitvPlane_->GetFrequency() = 0.025f;
	primitvPlane_->SetCount(40);
	primitvPlane_->SetPos({ 0,50,0 });
	primitvPlane_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	primitvPlane_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	//primitvPlane_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	primitvPlane_->SetLifeTimeMinMax(1, 3);
	//primitvPlane_->SetIsGravity(true);
	primitvPlane_->SetUsebillboard(false);
	primitvPlane_->SetIsAlpha(true);
	primitvPlane_->SetIsLifeTimeScale(true);
	primitvPlane_->SetColorMinMax({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	//primitvPlane_->SetIsRotateVelocity(true);
	//primitvPlane_->SetIsBounce(true);
	primitvPlane_->SetSizeMinMax(Vector3{ 0.1f,2.5f,0.1f }, { 0.1f ,5.0f,0.1f });



}

/// <summary>
///  ライトの初期化
/// </summary>
void TestScene::InitializeLight()
{
	PointLightData pointLightData;

	pointLightData.color = { 1.0f,1.0f,1.0f,1.0f };
	pointLightData.position = { 0.0f,-1.0f,-10.0f };
	pointLightData.radius = 10.0f;
	pointLightData.intensity = 10.0f;
	pointLightData.lig = 0.2f;
	pointLightData.isLight = true;
	pointLightData.decay = 1.0f;
	point = std::make_shared<PointLight>();

	point->point = pointLightData;

	LightManager::GetInstance()->AddLight(point);

	SpotLightData spotLightData;

	spotLightData = SpotLightData({ 1.0f,1.0f,1.0f,1.0f }, { 2.0f,1.25f,0.0f }, 10.0f, Normalize({ -1.0f,-1.0f,0.0f }), 10.0f, 2.0f, std::cos(std::numbers::pi_v<float> / 3.0f), 1.0f);
	spotLightData.position = Vector3(10, 0, 10);
	spotLightData.intensity = 50.0f;
	spotLightData.isLight = true;
	spotLightData.decay = 1.0f;

	spot = std::make_shared<SpotLight>();

	spot->spot = spotLightData;
	LightManager::GetInstance()->AddLight(spot);

	LineCommon::GetInstance()->SetDefaltCamera(camera.get());

	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 2.0f;
	directionalLightData.isLight = true;



	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	LightManager::GetInstance()->AddLight(directional);
}

/// <summary>
/// カメラの初期化 
/// </summary>
void TestScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();
	camera->Initialize();
	camera->transform_.rotate = { 1.0f,0,0 };
	camera->transform_.translate = { 0,100,-60.0f };

	debugCamera = std::make_unique<DebugCamera>();
	debugCamera->Initialize();
}

/// <summary>
/// その他初期化
/// </summary>
void TestScene::InitializeOthers()
{
	capsule_.radius = 10;
	capsule_.segment.origin = { 4,-5,4 };
	capsule_.segment.end = { 4,5,4 };



	octree = std::make_unique<Octree>(AABB({ -100,-100,-100 }, { 100,100,100 }), 0);
	octree->root->subdivide(4, 4, 4, 10);

	octree->insert(*stairObject->GetMesh(0));// メッシュ挿入


	world.Initialize();

	Box box;
	box.min_ = { 20,20 };
	box.max_ = { 30,30 };

	OBB2D obb2d{};
	obb2d.center = { 100,100 };
	obb2d.halfSize = { 100,10 };
	obb2d.axisZ.x = 1.0f;

	map->AddObstacleFromOBB2D(obb2d);

	map->AddObstacleFromBox(box);

	for (int i = 0; i < 500; i++) {



		Box boxs;
		boxs.min_.x = Random::RandomFloat(0, map->GetWidth() * map->GetCellSize());
		boxs.min_.y = Random::RandomFloat(0, map->GetHeight() * map->GetCellSize());
		boxs.max_.x = Random::RandomFloat(boxs.min_.x, boxs.min_.x + map->GetCellSize() * 3);
		boxs.max_.y = Random::RandomFloat(boxs.min_.y, boxs.min_.y + map->GetCellSize() * 3);


		if (IsCollision(AABB{ {boxs.min_.x,-20,boxs.min_.y},{boxs.max_.x,20,boxs.max_.y} }, goalObject->worldtransform_.translate_)) {
			continue;
		}
		if (IsCollision(AABB{ {boxs.min_.x,-20,boxs.min_.y},{boxs.max_.x,20,boxs.max_.y} }, playerObject->worldtransform_.translate_)) {
			continue;
		}


		map->AddObstacleFromBox(boxs);
	}






	pathfinder.SetMap(*map);

	playerObject->Update();
	goalObject->Update();

	Vector2 plyerPos = playerObject->GetWorldPosition().xz();
	Vector2 goalPos = goalObject->GetWorldPosition().xz();

	pathfinder.FindPath(plyerPos, goalPos, path);



	//for (int i = 0; i < map->GetWidth(); i++) {
	//	for (int j = 0; j < map->GetHeight(); j++) {

	//		// マップチップの種類を取得
	//		MapCellType cell = map->GetCell(i, j);

	//		// 障害物でないならスキップ
	//		if (cell != MapCellType::Obstacle) {
	//			continue;
	//		}

	//		// 障害物だった場合、オブジェクトを生成
	//		ObjectInstans obj{};
	//		obj.Initialize();
	//		obj.transform.translate_.x = float(map->GetCellSize() * i) + map->GetCellSize() / 2;
	//		obj.transform.translate_.y = 10;
	//		obj.transform.translate_.z = float(map->GetCellSize() * j) + map->GetCellSize() / 2;
	//		obj.transform.scale_ = map->GetCellSize() / 2.0f;

	//		int randdd = Random::RandomInt32_t(0, 0);

	//		// オブジェクトを種類ごとに配置
	//		if (randdd == 0) {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/uvChecker.png", obj);
	//		}
	//		else if (randdd == 1) {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/renga.png", obj);
	//		}
	//		else if (randdd == 2) {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/Image.png", obj);
	//		}
	//		else if (randdd == 3) {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/ground.png", obj);
	//		}
	//		else if (randdd == 4) {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/grass.png", obj);
	//		}
	//		else {
	//			Object3dInstansManager::GetInstance()->AddObject("BoxBox.obj", "resources/Texture/enemy.png", obj);
	//		}
	//	}
	//}

}

#pragma endregion 各初期化

#pragma region 各シーン初期化

void TestScene::InitializeRoom01()
{
}

void TestScene::InitializeRoom02()
{
}

void TestScene::InitializeRoom03()
{
}

void TestScene::InitializeRoom04()
{
}

void TestScene::InitializeRoom05()
{
}

void TestScene::InitializeRoom06()
{
	spot->spot.position = { -100,-100,-100 };
	point->point.position = { -100,-100,-100 };
}

void TestScene::InitializeRoom07()
{
}

void TestScene::InitializeRoom08()
{
}

#pragma endregion 

#pragma region 各シーン更新

void TestScene::UpdateRoom01()
{
	ocean_->Update();

	GetEntity3DManager()->GetSkyBoxCommon()->SetCamara(camera.get());
	GetEntity3DManager()->GetSkyBoxCommon()->Update();


	primitvPlane_->Update();
}

void TestScene::UpdateRoom02()
{
	GetSceneManager()->ChangeScene("GAMEPLAY");


	emitter_->Update();
	emitterEnemy_->Update();
}

void TestScene::UpdateRoom03()
{
	multiy.Update();
}

void TestScene::UpdateRoom04()
{
	skinningObject.UpdateSkinning();
	skinningObject2.UpdateSkinning();

	triCen;

	Vector3 a = { tri2d.vertices[0].x, 5 ,tri2d.vertices[0].y };
	Vector3 b = { tri2d.vertices[1].x, 5 ,tri2d.vertices[1].y };
	Vector3 c = { tri2d.vertices[2].x, 5 ,tri2d.vertices[2].y };


	LineCommon::GetInstance()->AddLine(a, b, { 1,1,1,1 });
	LineCommon::GetInstance()->AddLine(b, c, { 1,1,1,1 });
	LineCommon::GetInstance()->AddLine(c, a, { 1,1,1,1 });


	CornerSegment corner;// = { sphere2d.center }
	corner.center.x = sphere2d.center.x;
	corner.center.y = 5;
	corner.center.z = sphere2d.center.y;

	corner.radius = sphere2d.radius;
	corner.segment = 16;


	world.translate_.x = sphere2d.center.x;
	world.translate_.z = sphere2d.center.y;

	LineCommon::GetInstance()->AddLineCorner(corner, world);

	int size = GetEntity3DManager()->GetObject3dInstansManager()->GetSize();




	object_;

}

void TestScene::UpdateRoom05()
{
	LineCommon::GetInstance()->AddGrid(1000, 1000, 10, { 1,1,1,1 });
}

void TestScene::UpdateRoom06()
{

#ifdef _DEBUG

	ImGui::Begin("oc");
	ImGui::DragFloat3("div", &div_.x);
	ImGui::DragInt("maxDepth", &maxDepth);
	ImGui::DragInt("depth", &octree->root->depth);
	ImGui::DragFloat3("max", &octree->root->bounds.max_.x);
	ImGui::DragFloat3("min", &octree->root->bounds.min_.x);
	bool isColl = octree->checkCollisions(capsule_);
	ImGui::Checkbox("isColl", &isColl);
	if (ImGui::Button("clear")) {
		octree->root->clear();
	};
	if (ImGui::Button("Set")) {
		int x = static_cast<int>(div_.x);
		int y = static_cast<int>(div_.y);
		int z = static_cast<int>(div_.z);

		octree->root->subdivide(x, y, z, maxDepth);
	}
	ImGui::End();
	ImGui::Begin("capsule");
	ImGui::DragFloat("rad", &capsule_.radius, 0.1f);
	ImGui::DragFloat3("origin", &capsule_.segment.origin.x, 0.1f);
	ImGui::DragFloat3("end", &capsule_.segment.end.x, 0.1f);
	ImGui::End();
#endif // _DEBUG










	//stairObject->LineMesh();
	stairObject->Update();
	LineCommon::GetInstance()->AddLineCapsule(capsule_);
	octree->draw(*LineCommon::GetInstance());

}

void TestScene::UpdateRoom07()
{

	Vector2 plyerPos = playerObject->GetWorldPosition().xz();
	Vector2 goalPos = goalObject->GetWorldPosition().xz();
	int playerX, playerZ;
	int goalX, goalZ;

	// ワールド座標 -> マップ座標に変換
	bool validPlayer = map->WorldToMap(plyerPos.x, plyerPos.y, playerX, playerZ);
	bool validGoal = map->WorldToMap(goalPos.x, goalPos.y, goalX, goalZ);

	if (validPlayer && validGoal &&
		!map->IsBlocked(playerX, playerZ) &&
		!map->IsBlocked(goalX, goalZ))
	{
		pathfinder.FindPath(plyerPos, goalPos, path);


		// 進行方向を取得して正規化
		Vector2 direction = pathfinder.GetDirectionToNextNode().Normalize();

		// プレイヤーの位置を更新
		float speed = 1.0f;  // 任意の速度
		playerObject->worldtransform_.translate_.x += direction.x * speed;
		playerObject->worldtransform_.translate_.z += direction.y * speed;
	}





	Vector2 sosos = Input::GetInstance()->GetGamePadLeftStick();



	goalObject->worldtransform_.translate_.x += sosos.x * 2.0f;
	goalObject->worldtransform_.translate_.z += sosos.y * 2.0f;




#ifdef _DEBUG
	ImGui::Begin("dnadjas");
	ImGui::DragFloat3("goal", &goalObject->worldtransform_.translate_.x);
	ImGui::End();
#endif // _DEBUG




	playerObject->Update();
	goalObject->Update();

	pathfinder.DrawPath(11.0f);

	map->DrawMapChip(10.0f);
}

void TestScene::UpdateRoom08()
{

	noise->ImguiParameter();


#ifdef _DEBUG
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("NoiseSet")) {
		if (ImGui::Button("set")) {
			GetEntity3DManager()->GetObject3dInstansManager()->Clear("BoxBox.obj");

			for (int i = 0; i < map->GetWidth(); i++) {
				for (int j = 0; j < map->GetHeight(); j++) {
					ObjectInstans obj{};
					obj.Initialize();
					obj.transform.translate_.x = float(map->GetCellSize() * i) + map->GetCellSize() / 2;
					obj.transform.translate_.z = float(map->GetCellSize() * j) + map->GetCellSize() / 2;
					obj.transform.scale_ = map->GetCellSize() / 2.0f;


					float y = static_cast<float>(noise->PerlinNoise(float(i), float(j)) * map->GetCellSize() * 20);

					obj.transform.translate_.y = static_cast<float>(y);

					GetEntity3DManager()->GetObject3dInstansManager()->AddObject("BoxBox.obj", "resources/Texture/renga.png", obj);
				}
			}
		}
	}
	ImGui::End();


#endif // _DEBUG



}

void TestScene::SwitchRoom()
{
#ifdef _DEBUG
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("SceneRoom")) {
		if (ImGui::Button("Room01")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom01;
		}
		if (ImGui::Button("Room02")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom02;
		}
		if (ImGui::Button("Room03")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom03;
		}
		if (ImGui::Button("Room04")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom04;
		}
		if (ImGui::Button("Room05")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom05;
		}
		if (ImGui::Button("Room06")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom06;
		}
		if (ImGui::Button("Room07")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom07;
		}
		if (ImGui::Button("Room08")) {
			behaviorRequest_ = SceneBehavior::kSceneRoom08;
		}

	}
	ImGui::End();
#endif // _DEBUG

	if (input_->IsTriggerKey(DIK_1)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom01;
	}
	if (input_->IsTriggerKey(DIK_2)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom02;
	}
	if (input_->IsTriggerKey(DIK_3)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom03;
	}
	if (input_->IsTriggerKey(DIK_4)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom04;
	}
	if (input_->IsTriggerKey(DIK_5)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom05;
	}
	if (input_->IsTriggerKey(DIK_6)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom06;
	}
	if (input_->IsTriggerKey(DIK_7)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom07;
	}
	if (input_->IsTriggerKey(DIK_8)) {
		behaviorRequest_ = SceneBehavior::kSceneRoom08;
	}

}


#pragma endregion 

