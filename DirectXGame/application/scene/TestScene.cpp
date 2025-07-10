#include"TestScene.h"
//#include"DirectXGame/engine/struct/Quaternion.h"
#include "DirectXGame/engine/SkyBox/SkyBoxCommon.h"

#include "DirectXGame/engine/math/Random.h"

#include "DirectXGame/engine/MyGame/MyGame.h"

void TestScene::Initialize()
{
	//オーディオの初期化
	//audio_ = Audio::GetInstance();
	// 入力初期化
	input_ = GetInput();
	// ImGui初期化
	imGuiManager = GetDxCommon()->GetImGuiManager();


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


	loadData_ = std::make_unique<LoadLevelData>();
	loadData_->Initialize(GetEntity3DManager(), GetDxCommon()->GetModelManager(), camera.get(), "scene.json");


	if (!loadData_->GetLevelData()->players.empty()) {
		auto& playerData = loadData_->GetLevelData()->players[0];
		skinningObject->worldtransform_.translate_ = playerData.position;
		skinningObject->worldtransform_.rotate_ = playerData.rotation;
	}


	GlobalVariables* globalVariables = GetGlobalVariables();
	globalVariables->CreateGroup("ddd");
	globalVariables->AddItem("ddd","g_bool",g_bool);
	globalVariables->AddItem("ddd","g_int",g_int);
	globalVariables->AddItem("ddd","g_uint", g_uint);
	globalVariables->AddItem("ddd","g_float",g_float);
	globalVariables->AddItem("ddd","g_v2",g_v2);
	globalVariables->AddItem("ddd","g_v3",g_v3);
	globalVariables->AddItem("ddd","g_v4",g_v4);
	globalVariables->AddItem("ddd","g_string", g_string);
	globalVariables->AddItem("ddd","g_transform", g_transform);
	
	
	globalVariables->AddItem("aaa","g_aaaa", g_aaaa);

	AppGlobalVariables();
}

void TestScene::Finalize()
{

}

void TestScene::Update()
{
	SwitchRoom(); // 部屋切り替え
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());

	loadData_->Update();

	timer_.DebugImGui("timer");

	timer_.Update(MyGame::GameTime());

#ifdef _DEBUG

	

	if (isDebugCamera) {
		debugCamera->Update();

		camera->viewMatrix_ = debugCamera->GetViewProjection().viewMatrix_;
		camera->projectionMatrix_ = debugCamera->GetViewProjection().projectionMatrix_;
		camera->transform_ = debugCamera->GetViewProjection().transform_;
	}
#endif // _DEBUG

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
	
	camera->UpdateMatrix();

}

void TestScene::Draw3D()
{
	
	switch (behavior_)
	{
	case TestScene::SceneBehavior::kSceneRoom01:
		
		//skyBoxObject->Draw();
		//skyBoxObject2->Draw();
		//
		//tail.Draw();
		//
		//oceanObject->Draw();

		//multiy->Draw();



		break;
	case TestScene::SceneBehavior::kSceneRoom02:
		//tail.Draw();

		//primitiveObject3d->Draw();

		break;
	case TestScene::SceneBehavior::kSceneRoom03:
		//taleObject->Draw();
		break;
	case TestScene::SceneBehavior::kSceneRoom04:
		//tail.Draw();

		//skinningObject->Draw();
		//skinningObject2->Draw();
		//skinningObject3->Draw();
		break;
	case TestScene::SceneBehavior::kSceneRoom05:
		break;
	case TestScene::SceneBehavior::kSceneRoom06:

		break;
	case TestScene::SceneBehavior::kSceneRoom07:

		//// プレイヤー
		//playerObject->Draw();

		//// ゴール
		//goalObject->Draw();


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

void TestScene::AppGlobalVariables()
{
	GlobalVariables* globalVariables = GetGlobalVariables();
	g_bool = globalVariables->GetValue<bool>("ddd", "g_bool");
	g_int = globalVariables->GetValue<int>("ddd", "g_int");
	g_uint = globalVariables->GetValue<uint32_t>("ddd", "g_uint");
	g_float = globalVariables->GetValue<float>("ddd", "g_float");
	g_v2 = globalVariables->GetValue<Vector2>("ddd", "g_v2");
	g_v3 = globalVariables->GetValue<Vector3>("ddd", "g_v3");
	g_v4 = globalVariables->GetValue<Vector4>("ddd", "g_v4");
	g_string = globalVariables->GetValue<std::string>("ddd", "g_string");
	g_transform = globalVariables->GetValue<Transform>("ddd", "g_transform");
	
	g_aaaa = globalVariables->GetValue<bool>("aaa", "g_aaaa");

}


#pragma region Initialize

/// <summary>
/// 3Dオブジェクトの初期化
/// </summary>
void TestScene::InitializeObject3D()
{
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());

	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize(GetEntity3DManager(), { 10000,10000 });
	ocean_->GetMaterial()->enableLighting_ = false;


	oceanObject = std::make_unique<Object3d>();
	oceanObject->Initialize(GetEntity3DManager(), Object3d::ObjectModelType::kOcean);
	oceanObject->SetCamera(camera.get());
	oceanObject->SetOcean(ocean_.get());
	oceanObject->worldtransform_.translate_ = { 0,-30,0 };
	oceanObject->worldtransform_.rotate_.x = DegreesToRadians(90);
	oceanObject->SetObjectDrawType(Object3d::ObjectDrawType::kTranslucent03);
	oceanObject->SetIsDraw(true);
	

	skinningObject = GetEntity3DManager()->CreateObject3D("KnightCharacter", Object3d::ObjectModelType::kSkinning, {30,1,1}, camera.get());
	skinningObject->SetModel("KnightCharacter.gltf");
	skinningObject->worldtransform_.translate_ = { 30,1,1 };
	skinningObject->worldtransform_.scale_ = { 1,1,1 };
	skinningObject->SetIsDraw(true);

	skinningObject2 = GetEntity3DManager()->CreateObject3D("BoxBox", Object3d::ObjectModelType::kNormal, { 0,0,0 }, camera.get());
	skinningObject2->SetModel("BoxBox.obj");
	skinningObject2->worldtransform_.scale_ = { 1,1,1 };
	skinningObject2->SetIsDraw(true);
	//skinningObject2->worldtransform_.SetParent(&Animetion::FindJointByName(skinningObject->model->modelData.skeleton,"LowerArm.R")->skeletonSpaceMatrix);
	//skinningObject2->worldtransform_.SetParent(Animetion::GetWorldMatrixOfJoint(skinningObject->model->modelData.skeleton, "LowerArm.R", skinningObject->worldtransform_.worldMat_));

	skinningObject3 = std::make_unique<Object3d>();
	skinningObject3->Initialize(GetEntity3DManager(), Object3d::ObjectModelType::kNormal);
	skinningObject3->SetModel("d.gltf");
	skinningObject3->worldtransform_.translate_ = { -30,10,1 };
	skinningObject3->worldtransform_.scale_ = { 10,10,10 };
	skinningObject3->SetCamera(camera.get());
	skinningObject3->SetName("testBri");
	skinningObject3->SetIsDraw(false);

	tail.Initialize(GetEntity3DManager());
	tail.SetModel("stage.gltf");
	tail.SetCamera(camera.get());
	tail.SetName("tail");
	tail.GetMaterial(0)->shininess_ = 1000.0f;
	tail.worldtransform_.translate_.y = -10.0f;
	tail.SetIsDraw(false);

	multiy = std::make_unique<Object3d>();
	multiy->Initialize(GetEntity3DManager());
	multiy->SetModel("multiMaterial.gltf");
	multiy->SetCamera(camera.get());
	multiy->worldtransform_.scale_ = { 10,10,10 };
	multiy->SetName("multiy");
	multiy->SetIsDraw(false);

	/// 階段
	stairObject = std::make_unique<Object3d>();
	stairObject->Initialize(GetEntity3DManager());
	stairObject->SetModel("stair.obj");
	stairObject->SetCamera(camera.get());
	stairObject->SetIsDraw(false);


	taleObject = std::make_unique<Object3d>();
	taleObject->Initialize(GetEntity3DManager());
	taleObject->SetModel("terrain.obj");
	taleObject->SetCamera(camera.get());
	taleObject->worldtransform_.scale_ = 10.0f;
	taleObject->SetIsDraw(false);
	
	ShapeParameter::Ring ring;
	ring.innerRadius = 1.0f;
	ring.outerRadius = 7.0f;
	ring.segments = 16;
	

	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(GetEntity3DManager(), "resources/Texture/hdr/sky.dds");
	
	skyBox2 = std::make_unique<SkyBox>();
	skyBox2->Initialize(GetEntity3DManager(), "resources/Texture/hdr/sky.dds");

	
	skyBoxObject = std::make_unique<Object3d>();
	skyBoxObject->Initialize(GetEntity3DManager(), Object3d::ObjectModelType::kSkyBox);
	skyBoxObject->SetSkyBox(skyBox.get());
	skyBoxObject->SetCamera(camera.get());
	skyBoxObject->worldtransform_.scale_ = {10,10,10};
	skyBoxObject->SetName("skyBox");
	skyBoxObject->SetIsDraw(true);
	
	
	skyBoxObject2 = std::make_unique<Object3d>();
	skyBoxObject2->Initialize(GetEntity3DManager(), Object3d::ObjectModelType::kSkyBox);
	skyBoxObject2->SetSkyBox(skyBox2.get());
	skyBoxObject2->SetCamera(camera.get());
	skyBoxObject2->worldtransform_.scale_ = {1,1,1};
	skyBoxObject2->SetName("skyBox2");
	skyBoxObject2->SetIsDraw(false);


	ShapeParameter::Cylinder cylinderParam;
	cylinderParam.height = 10.0f;
	cylinderParam.innerRadius = 2.1f;
	cylinderParam.outerRadius = 2.1f;
	cylinderParam.isCover = false;
	cylinderParam.segments = 16;

	//primitiveCylinder_ = std::make_unique<Primitive>();
	//primitiveCylinder_->Initialize<ShapeParameter::Cylinder>(GetEntity3DManager()->GetPrimitiveCommon(), Primitive::ShapeType::Cylinder, cylinderParam, "resources/Texture/effect/gradationLine.png");


	//hitObject_ = std::make_unique<Object3d>();
	//hitObject_->Initialize(GetEntity3DManager(), Object3d::ObjectType::kPrimitive);
	//hitObject_->SetPrimitive(primitiveCylinder_.get());
	//hitObject_->SetName("cylinder");
	//hitObject_->SetIsDraw(false);


	primiPlane = std::make_unique<Primitive>();
	ShapeParameter::ShapePlane shapePlane;
	primiPlane->Initialize<ShapeParameter::ShapePlane>(GetEntity3DManager()->GetPrimitiveCommon(), Primitive::ShapeType::Plane, shapePlane, "resources/Texture/uvChecker.png");

}

/// <summary>
/// スプライトの初期化
/// </summary>
void TestScene::InitializeObject2D()
{
	
	for (int i = 0; i < 4; i++)
	{
		auto sprite = std::make_unique<Sprite>();

		sprite->Initialize(GetEntity2DManager()->GetSpriteCommon(), "resources/Texture/uvChecker.png", false);
		sprite->SetTextureSize({ 64,64 });
		sprite->SetSize({ 128,128 });
		sprite->SetAnimeSize({ 64,64 });
		sprite->SetMaxAnimeNum({ 8,8 });
		sprite->SetPosition({ static_cast<float>(i) * (128 + 10) ,0 });

		sprite_.push_back(std::move(sprite));
	}

	///
	primitive2d1_ = std::make_unique<Primitive2D>();
	primitive2d1_->Initialize(GetEntity2DManager()->GetSpriteCommon(),Primitive2D::ShapeType::Ring, { 1,1,1,1 });
	primitive2d1_->position = { 640,360 };
	//primitive2d1_->rotation = DegreesToRadians(45);

}

/// <summary>
/// パーティクルの初期化
/// </summary>
void TestScene::InitializeParticle()
{
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());
	
	//GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(camera.get());
	
	//GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetMesh(primiPlane->GetMesh());
	
	
	/*GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kAcceleration,Field::ShapeType::kAABB,"加速");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kDestruction, Field::ShapeType::kAABB,"破棄");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kColor, Field::ShapeType::kAABB,"色");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kGravity, Field::ShapeType::kAABB,"重力");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kGravity, Field::ShapeType::kSphere,"重力");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kDeceleration, Field::ShapeType::kAABB,"減速");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kNoise, Field::ShapeType::kAABB,"ノイズ");
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(Field::EffectType::kTornado, Field::ShapeType::kAABB,"竜巻");*/

	

	emitter_ = std::make_unique<ParticleEmitter>();
	emitter_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),"emitter", "cc", ParticleData::SpawnType::kSpline);
	emitter_->GetFrequency() = 0.1f;
	emitter_->SetCount(1);
	//emitter_->SetParent(skinningObject2->worldtransform_);
	emitter_->SetRotateMinMax(-Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	emitter_->SetPos({ 0,10,0 });
	emitter_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	emitter_->SetLifeTimeMinMax(1.0f, 2.0f);
	emitter_->SetIsGravity(true);
	emitter_->SetIsAlpha(true);
	emitter_->AddControlPoints(Vector3{ 0,0,0 });
	emitter_->AddControlPoints(Vector3{ 10,10,0 });
	emitter_->AddControlPoints(Vector3{ 20,20,0 });
	emitter_->AddControlPoints(Vector3{ 30,30,0 });


	primitvPa_ = std::make_unique<ParticleEmitter>();
	primitvPa_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),"emitterPPPP", "cc", ParticleData::SpawnType::kAABB);
	primitvPa_->GetFrequency() = 0.1f;
	primitvPa_->SetCount(1);
	primitvPa_->SetRotateMinMax(-Vector3{ 1.0f,1.0f,1.0f }, { 1.0f,1.0f,1.0f });
	primitvPa_->SetPos({ 0,10,0 });
	primitvPa_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	primitvPa_->SetLifeTimeMinMax(1.0f, 2.0f);
	




	emitterEnemy_ = std::make_unique<ParticleEmitter>();
	emitterEnemy_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),"emitterPrimi", "primi");
	emitterEnemy_->GetFrequency() = 0.1f;
	emitterEnemy_->SetCount(1);
	emitterEnemy_->SetPos({ 0,50,0 });
	emitterEnemy_->SetVelocityMinMax({ -10,20,-10 }, { 10, 50, 10 });
	emitterEnemy_->SetRengeMinMax({ -1,0,-1 }, { 1, 10, 1 });
	emitterEnemy_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	emitterEnemy_->SetRotateVelocityMinMax(-Vector3{ 0.1f,0.1f,0.1f }, { 0.1f,0.1f,0.1f });
	//emitterEnemy_->SetColorMinMax();
	emitterEnemy_->SetLifeTimeMinMax(5, 10);
	emitterEnemy_->SetIsGravity(true);
	emitterEnemy_->SetUsebillboard(false);
	emitterEnemy_->SetIsAlpha(true);
	emitterEnemy_->SetIsLifeTimeScale(true);
	emitterEnemy_->SetIsRotateVelocity(true);
	emitterEnemy_->SetIsBounce(true);
	emitterEnemy_->SetSizeMinMax(Vector3{ 0.1f,0.1f,0.1f }, { 0.2f,0.2f,0.2f });
	emitterEnemy_->SetUseFieldName({"All"});

	fieldEffect_ = std::make_unique <Field::FieldEffect >();
	fieldEffect_->Initialize("fieldEffect", Field::ShapeType::kAABB, Field::EffectType::kAcceleration, GetEntity3DManager()->Get3DLineCommon());
	fieldEffect_->SetParent(emitterEnemy_->transform_);
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->AddFieldEffect(fieldEffect_.get());

	primitvPlane_ = std::make_unique<ParticleEmitter>();
	primitvPlane_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),"primiPlane", "primiPlane", ParticleData::SpawnType::kPoint);
	primitvPlane_->GetFrequency() = 0.025f;
	primitvPlane_->SetCount(40);
	//primitvPlane_->SetPos({ 0,50,0 });
	primitvPlane_->SetVelocityMinMax({ 0,0,0 }, { 0, 0, 0 });
	primitvPlane_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	primitvPlane_->SetLifeTimeMinMax(1, 3);
	primitvPlane_->SetUsebillboard(false);
	primitvPlane_->SetIsAlpha(true);
	primitvPlane_->SetIsLifeTimeScale(true);
	primitvPlane_->SetColorMinMax({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	primitvPlane_->SetSizeMinMax(Vector3{ 0.1f,2.5f,0.1f }, { 0.1f ,5.0f,0.1f });
	primitvPlane_->SetParent(skinningObject2->worldtransform_);

	primitvPlaneSmoke_ = std::make_unique<ParticleEmitter>();
	primitvPlaneSmoke_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),"smokePlane01", "smokePlane01", ParticleData::SpawnType::kPoint);
	primitvPlaneSmoke_->GetFrequency() = 0.025f;
	primitvPlaneSmoke_->SetCount(3);
	primitvPlaneSmoke_->SetPos({ 0,50,0 });
	primitvPlaneSmoke_->SetVelocityMinMax({ 0,0,0 }, { 0, 10, 0 });
	primitvPlaneSmoke_->SetRotateMinMax(-DegreesToRadians(Vector3{ 90,90,90 }), DegreesToRadians(Vector3{ 90,90,90 }));
	primitvPlaneSmoke_->SetLifeTimeMinMax(1, 3);
	primitvPlaneSmoke_->SetUsebillboard(false);
	primitvPlaneSmoke_->SetEnableLighting(false);
	primitvPlaneSmoke_->SetIsAlpha(true);
	primitvPlaneSmoke_->SetIsLifeTimeScale(true);
	primitvPlaneSmoke_->SetLifeTimeScaleTopBottom(ParticleData::TopBottom::kTop);

	primitvPlaneSmoke_->SetColorMinMax({ 1.0f ,1.0f ,1.0f ,1.0f }, { 1.0f,1.0f,1.0f,1.0f });
	primitvPlaneSmoke_->SetIsRotateVelocity(true);
	primitvPlaneSmoke_->SetAlphaClipping(0.25f);
	primitvPlaneSmoke_->SetSizeMinMax(Vector3{ 5.0f,5.0f,1.0f }, { 5.0f ,5.0f,1.0f });
	primitvPlaneSmoke_->SetUvTransformVeloctiy({{0.0f,0,0},{},{0.0f,0.0f,0}});


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

	//GetEntity3DManager()->GetLightManager()->AddLight(point);

	SpotLightData spotLightData;

	spotLightData = SpotLightData({ 1.0f,1.0f,1.0f,1.0f }, { 2.0f,1.25f,0.0f }, 10.0f, Normalize({ -1.0f,-1.0f,0.0f }), 10.0f, 2.0f, std::cos(std::numbers::pi_v<float> / 3.0f), 1.0f);
	spotLightData.position = Vector3(10, 0, 10);
	spotLightData.intensity = 50.0f;
	spotLightData.isLight = true;
	spotLightData.decay = 1.0f;

	spot = std::make_shared<SpotLight>();

	spot->spot = spotLightData;
	//GetEntity3DManager()->GetLightManager()->AddLight(spot);

	GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(camera.get());

	SetCamera(camera.get());
	
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 0.9f;
	directionalLightData.isLight = true;



	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	GetEntity3DManager()->GetLightManager()->AddLight(directional);

}

/// <summary>
/// カメラの初期化 
/// </summary>
void TestScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();
	camera->Initialize(GetEntity3DManager()->GetCameraCommon());
	camera->transform_.rotate = { 1.0f,0,0 };
	camera->transform_.translate = { 0,100,-60.0f };
	//camera->SetNearClip();
	camera->SetFarClip(10000.0f);



	debugCamera = std::make_unique<DebugCamera>();
	debugCamera->Initialize(GetEntity3DManager()->GetCameraCommon());
}

/// <summary>
/// その他初期化
/// </summary>
void TestScene::InitializeOthers()
{
	

	octree = std::make_unique<Octree>(AABB({ -100,-100,-100 }, { 100,100,100 }), 0);
	octree->root->subdivide(4, 4, 4, 10);

	octree->insert(*stairObject->GetMesh(0));// メッシュ挿入
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
	skinningObject2->worldtransform_.SetParent(Animetion::GetWorldMatrixOfJoint(skinningObject->model->modelData.skeleton, "MiddleHand.R", skinningObject->worldtransform_.worldMat_));
	//emitter_->Update();
	primitvPlane_->Update();
	ImGui::Begin("Animetion");
	if (ImGui::Button("Idle")) {
		skinningObject->SetAnimetion("Idle",0.3f);
	}
	if (ImGui::Button("Run")) {
		skinningObject->SetAnimetion("Run", 0.3f);
	}
	ImGui::End();
}

void TestScene::UpdateRoom02()
{
	if (input_->IsTriggerKey(DIK_P)) {
		GetSceneManager()->ChangeScene("GAMEPLAY");
	}

	emitter_->Update();
	emitterEnemy_->Update();
	primitvPa_->Update();
}

void TestScene::UpdateRoom03()
{
	
}

void TestScene::UpdateRoom04()
{
	ImGui::Begin("papapa");
	if(ImGui::Button("true")) {
		primitvPlaneSmoke_->SetEnableLighting(true);
	}
	if(ImGui::Button("false")) {
		primitvPlaneSmoke_->SetEnableLighting(false);
	}
	ImGui::DragFloat("clipping", &clipping_,0.01f);
	primitvPlaneSmoke_->SetAlphaClipping(clipping_);
	ImGui::End();
	primitvPlaneSmoke_->Update();


	
}

void TestScene::UpdateRoom05()
{
	GetEntity3DManager()->Get3DLineCommon()->AddGrid(1000, 1000, 10, { 1,1,1,1 });
}

void TestScene::UpdateRoom06()
{
}

void TestScene::UpdateRoom07()
{

}

void TestScene::UpdateRoom08()
{


}

void TestScene::SwitchRoom()
{
#ifdef _DEBUG
	
	if (!ImGui::Begin("File", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}
	if (!ImGui::BeginMenuBar())
		return;

	if (ImGui::BeginMenu("SceneRoom")) {
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
		if (ImGui::Button("GameScene")) {
			GetSceneManager()->ChangeScene("GAMEPLAY");
		}

		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
	ImGui::End();
#endif // _DEBUG



}


#pragma endregion 

