#include "SampleScene.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Math/Random.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

// 初期化
void SampleScene::Initialize() {

	// 入力
	input_ = GetInput();
	
	// カメラ初期化
	InitCamera();
	// オブジェクト初期化
	InitObject();
	// スプライト初期化
	InitSprite();
	// パーティクル初期化
	InitParticle();
	// ライト初期化
	InitLight();
};

// カメラ
void SampleScene::InitCamera() {
	// シーンで使用する入力管理を一括初期化する
	inputCoordinator_ = std::make_unique<InputCoordinator>();
	inputCoordinator_->Initialize(GetInput());

	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables(), {});
	fixedCamera_->Update();
	// カメラ管理
	cameraManager_ = std::make_unique<CameraManager>();
	cameraManager_->Initialize(inputCoordinator_->GetInputSystem(), GetEntityManager(), GetGlobalVariables());
	// カメラ追加
	cameraManager_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");

	cameraManager_->SetUseCamera("fixedCamera", 0.0f);
	cameraManager_->Update();
	SetCamera(cameraManager_->GetCamera());
};
// ライト
void SampleScene::InitLight() {

	// 平行光源パラメータ設定
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 1.5f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;


	// 平行光源初期化
	directional = std::make_shared<Engine::DirectionalLight>();
	directional->directional = directionalLightData;

	// ライト管理クラスに追加
	GetEntityManager()->GetLightManager()->AddLight(directional);
};
// パーティクル
void SampleScene::InitParticle() {
	// パーティクルエミッターの生成
	aabbParticleEmitter_ = std::make_unique<Engine::AABBParticleEmitter>();
	aabbParticleEmitter_->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(),GetGlobalVariables(), "testAABBEmit", "testParticle");

	// パーティクルエミッターの生成
	sphereParticleEmitter = std::make_unique<Engine::SphereParticleEmitter>();
	sphereParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testSphereEmit", "testParticle");
	sphereParticleEmitter->transform_.translate_.x = 5;

	// パーティクルエミッターの生成
	cornerParticleEmitter = std::make_unique<Engine::CornerParticleEmitter>();
	cornerParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testCornerEmit", "testParticle");
	cornerParticleEmitter->transform_.translate_.x = 10;

	// パーティクルエミッターの生成
	splineParticleEmitter = std::make_unique<Engine::SplineParticleEmitter>();
	splineParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testSplineEmit", "testParticle");
	splineParticleEmitter->AddControlPoints({ 0,0,0 });
	splineParticleEmitter->AddControlPoints({ 1,1,1 });
	splineParticleEmitter->AddControlPoints({ 2,2,3 });
	splineParticleEmitter->AddControlPoints({ 3,3,7 });
	splineParticleEmitter->transform_.translate_.x = 15;

	// パーティクルエミッターの生成
	lineParticleEmitter = std::make_unique<Engine::LineParticleEmitter>();
	lineParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testLineEmit", "testParticle");
	lineParticleEmitter->transform_.translate_.x = 20;

	// パーティクルエミッターの生成
	triangleParticleEmitter = std::make_unique<Engine::TriangleParticleEmitter>();
	triangleParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testTriangleEmit", "testParticle");
	triangleParticleEmitter->transform_.translate_.x = -5;

	// パーティクルエミッターの生成
	meshParticleEmitter = std::make_unique<Engine::MeshParticleEmitter>();
	meshParticleEmitter->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testMeshEmit", "testParticle");
	meshParticleEmitter->SetMesh(GetDxCommon()->GetModelManager()->FindModel("teapot.obj")->GetModelData().mesh[0].get());
	meshParticleEmitter->transform_.translate_.x = -10;
	meshParticleEmitter->SetSize({ 0.05f,0.05f ,0.05f }, {});


	sprite2dP_ = std::make_unique<Engine::Sprite>();
	sprite2dP_->Initialize(GetEntityManager()->GetSpriteCommon(), "resources/Texture/icon/Clear_Sponge.png");
	sprite2dP_->Update();


	GetEntityManager()->GetEffectManager()->GetParticleManager2d()->SetCamera(GetCamara());
	particleEmitter2d_ = std::make_unique<Engine::ParticleEmitter2d>();
	particleEmitter2d_->Initialize(GetEntityManager()->GetEffectManager()->GetParticleManager2d(), "2d", "testParticle2d");
	particleEmitter2d_->SetCount(10, 0);
	particleEmitter2d_->SetAcceleration({ 0,9.8f }, {});
	particleEmitter2d_->GetEmitData().isEmit = true;
	particleEmitter2d_->SetPos({ 640,360 });

};
// オブジェクト
void SampleScene::InitObject() {
	object3dInstanceManager_ = GetEntityManager()->GetObject3dInstanceManager();


	// カメラセット
	object3dInstanceManager_->SetCamera(GetCamara());


	// 3Dオブジェクトの生成(オブジェクト名、モデルタイプ、位置、カメラ)
	testObject_ = GetEntityManager()->CreateObject3D("test1", Engine::ObjectModelType::kNormal, {}, nullptr);
	testObject_->SetModel("Player.obj");


	testObject_->GetWorldTransform().translate_ = { 0.0f, 10.0f, 5.0f };
	testObject_->GetRenderComponent()->SetObjectDrawType(Engine::ObjectDrawType::kTranslucent03);
	std::vector<int> map = {
		1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,3,0,0,0,1,
		1,0,0,0,0,3,0,0,0,1,
		1,0,2,0,0,0,0,0,0,1,
		1,0,2,0,3,0,0,0,0,1,
		1,0,2,0,3,0,0,0,0,1,
		1,0,2,0,3,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,
	};

	std::vector<MapId> mapId = {
		{1, "resources/Texture/uvChecker.png"},
		{2, "resources/Texture/grass.png"},
		{3, "resources/Texture/renga.png"}
	};

	
	// スカイボックスの生成
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(GetEntityManager(), "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = GetEntityManager()->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, cameraManager_->GetCamera());
	sky_->GetWorldTransform().scale_ = { 100,100,100 };
	sky_->SetSkyBox(skyBox.get());
};
//
void SampleScene::InitSprite() {

	sprite_ = std::make_unique<Engine::Sprite>();
	
	
	// スプライトの初期化
	sprite_->Initialize(GetEntityManager()->GetSpriteCommon(), "resources/Texture/uvChecker.png");
	sprite_->SetColor({1,1,1,1});			// 色
	sprite_->SetPosition({ 10,10 });		// 位置
	sprite_->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント
	
};


// 終了
void SampleScene::Finalize() {
	// パーティクルとスプライトはカメラ参照を持つため先に破棄する。
	particleEmitter2d_.reset();
	meshParticleEmitter.reset();
	triangleParticleEmitter.reset();
	lineParticleEmitter.reset();
	splineParticleEmitter.reset();
	cornerParticleEmitter.reset();
	sphereParticleEmitter.reset();
	aabbParticleEmitter_.reset();
	sprite2dP_.reset();
	sprite_.reset();
	plane_.reset();
	skyBox.reset();
	directional.reset();
	collisionManager2d_.reset();
	collisionManager_.reset();
	audioTest_.reset();

	// 生ポインタはEntityManager所有なので、シーン側では参照だけ無効化する。
	testObject_ = nullptr;
	sky_ = nullptr;
	object3dInstanceManager_ = nullptr;

	// CameraManagerが描画系へ渡した参照を外してからカメラ本体を破棄する。
	if (cameraManager_) {
		cameraManager_->Finalize();
		cameraManager_.reset();
	}
	fixedCamera_.reset();
	inputCoordinator_.reset();
	input_ = nullptr;

};

// 毎フレーム更新
void SampleScene::Update() {
	// シーン内の入力を一括更新する
	inputCoordinator_->Update(GetTime());

	// カメラ管理の更新
	cameraManager_->Update();
	SetCamera(cameraManager_->GetCamera());
	
	aabbParticleEmitter_->Update();
	

	particleEmitter2d_->Update();

	sprite_->Update();
};

// 描画
void SampleScene::Draw3D() {
};

void SampleScene::Draw2D() {
	sprite_->Draw();

};
