#include"TestScene.h"
//#include"DirectXGame/engine/struct/Quaternion.h"
#include "DirectXGame/engine/SkyBox/SkyBoxCommon.h"

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

	// 2Dオブジェクトの初期化
	InitializeObject3D();

	// 2Dオブジェクトの初期化
	InitializeObject2D();

	// パーティクルの初期化
	InitializeParticle();

	// ライトの初期化
	InitializeLight();

}

void TestScene::Finalize()
{

}

void TestScene::Update()
{


#ifdef _DEBUG
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



	ocean_->Update();
	emitter_->Update();
	emitterEnemy_->Update();

	mm.UpdateSkinning();
	mm2.Update();
	mm2.UpdateSkinning();
	tail.Update();
	multiy.Update();



	SkyBoxCommon::GetInstance()->SetCamara(camera.get());
	SkyBoxCommon::GetInstance()->Update();

}

void TestScene::Draw3D()
{
	tail.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);
	//mm.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_SOLID_BACK);
	//mm.Draw(Object3d::ObjectType::NoUvInterpolation_MODE_WIREFRAME_NONE);
	////mm.DrawLine();

	////mm2.DrawSkinning(Object3d::ObjectType::UvInterpolation_MODE_SOLID_BACK);

	////multiy.Draw();

	ocean_->Draw();
}

void TestScene::Draw2D()
{

	for (int i = 0; i < sprite_.size(); i++) {
		sprite_[i]->UpdateAmimetion(0.05f);

	}
	sprite_[0]->Draw();
	sprite_[1]->Draw(Sprite::SpriteType::NoUvInterpolation_MODE_SOLID);
	sprite_[2]->Draw(Sprite::SpriteType::UvInterpolation_MODE_WIREFRAME);
	sprite_[3]->Draw(Sprite::SpriteType::NoUvInterpolation_MODE_WIREFRAME);

}


#pragma region Initialize

/// <summary>
/// 3Dオブジェクトの初期化
/// </summary>
void TestScene::InitializeObject3D()
{
	Object3dCommon::GetInstance()->SetDefaltCamera(camera.get());

	ocean_ = std::make_unique<Ocean>();
	ocean_->Initialize({ 100,100 });
	ocean_->SetCamera(camera.get());
	ocean_->transform.rotate.x = DegreesToRadians(90);
	ocean_->material->color.a = 0.99f;

	// 列車オブジェクトを unique_ptr で作成
	mm.Initialize();
	//mm.SetModel("building.obj");
	mm.SetModel("iku.gltf");
	mm.worldtransform_.translate_ = { 30,1,1 };
	mm.worldtransform_.scale_ = { 10,10,10 };
	mm.SetCamera(camera.get());
	mm2.Initialize();
	mm2.SetModel("walk.gltf");
	//mm2.SetModel("AnimatedCube.gltf");
	mm2.worldtransform_.translate_ = { -30,10,1 };
	mm2.worldtransform_.scale_ = { 10,10,10 };
	mm2.SetCamera(camera.get());


	tail.Initialize();
	tail.SetModel("renga.gltf");
	tail.SetCamera(camera.get());
	tail.model->modelData.material[0]->shininess_ = 1000.0f;

	multiy.Initialize();
	multiy.SetModel("multiMaterial.gltf");
	multiy.SetCamera(camera.get());
	multiy.worldtransform_.scale_ = { 10,10,10 };
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

#pragma endregion 各初期化