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
	// オーディオ
	
	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables(), {});
	fixedCamera_->Update();
	// カメラ管理
	cameraManeger_ = std::make_unique<CameraManager>();
	cameraManeger_->Initialize(input_, GetEntity3DManager(), GetGlobalVariables());
	// カメラ追加
	cameraManeger_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");

	cameraManeger_->SetUseCamera("fixedCamera", 0.0f);
	cameraManeger_->Update();
	SetCamera(cameraManeger_->GetCamera());
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
	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	// ライト管理クラスに追加
	GetEntity3DManager()->GetLightManager()->AddLight(directional);
};
// パーティクル
void SampleScene::InitParticle() {
	// パーティクルエミッターの生成
	aabbParticleEmitter_ = std::make_unique<AABBParticleEmitter>();
	aabbParticleEmitter_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(),GetGlobalVariables(), "testAABBEmit", "testParticle");

	// パーティクルエミッターの生成
	sphereParticleEmitter = std::make_unique<SphereParticleEmitter>();
	sphereParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testSphereEmit", "testParticle");
	sphereParticleEmitter->transform_.translate_.x = 5;

	// パーティクルエミッターの生成
	cornerParticleEmitter = std::make_unique<CornerParticleEmitter>();
	cornerParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testCornerEmit", "testParticle");
	cornerParticleEmitter->transform_.translate_.x = 10;

	// パーティクルエミッターの生成
	splineParticleEmitter = std::make_unique<SplineParticleEmitter>();
	splineParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testSplineEmit", "testParticle");
	splineParticleEmitter->AddControlPoints({ 0,0,0 });
	splineParticleEmitter->AddControlPoints({ 1,1,1 });
	splineParticleEmitter->AddControlPoints({ 2,2,3 });
	splineParticleEmitter->AddControlPoints({ 3,3,7 });
	splineParticleEmitter->transform_.translate_.x = 15;

	// パーティクルエミッターの生成
	lineParticleEmitter = std::make_unique<LineParticleEmitter>();
	lineParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testLineEmit", "testParticle");
	lineParticleEmitter->transform_.translate_.x = 20;

	// パーティクルエミッターの生成
	triangleParticleEmitter = std::make_unique<TriangleParticleEmitter>();
	triangleParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testTriangleEmit", "testParticle");
	triangleParticleEmitter->transform_.translate_.x = -5;

	// パーティクルエミッターの生成
	meshParticleEmitter = std::make_unique<MeshParticleEmitter>();
	meshParticleEmitter->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager(), GetGlobalVariables(), "testMeshEmit", "testParticle");
	meshParticleEmitter->SetMesh(GetDxCommon()->GetModelManager()->FindModel("teapot.obj")->modelData.mesh[0].get());
	meshParticleEmitter->transform_.translate_.x = -10;
	meshParticleEmitter->SetSize({ 0.05f,0.05f ,0.05f }, {});


	sprite2dP_ = std::make_unique<Sprite>();
	sprite2dP_->Initialize(GetEntity2DManager()->GetSpriteCommon(), "resources/Texture/icon/Clear_Sponge.png");
	sprite2dP_->Update();


	GetEntity3DManager()->GetEffectManager()->GetParticleManager2d()->SetCamera(GetCamara());
	particleEmitter2d_ = std::make_unique<ParticleEmitter2d>();
	particleEmitter2d_->Initialize(GetEntity3DManager()->GetEffectManager()->GetParticleManager2d(), "2d", "testParticle2d");
	particleEmitter2d_->SetCount(10, 0);
	particleEmitter2d_->SetAcceleration({ 0,9.8f }, {});
	particleEmitter2d_->GetEmitData().isEmit = true;
	particleEmitter2d_->SetPos({ 640,360 });

};
// オブジェクト
void SampleScene::InitObject() {
	object3dInstansManager_ = GetEntity3DManager()->GetObject3dInstansManager();


	// カメラセット
	object3dInstansManager_->SetCamera(GetCamara());


	// 3Dオブジェクトの生成(オブジェクト名、モデルタイプ、位置、カメラ)
	testObject_ = GetEntity3DManager()->CreateObject3D("test1", ObjectModelType::kNormal, {}, nullptr);
	testObject_->SetModel("Player.obj");


	testObject_->GetWorldTransform().translate_ = { 0.0f, 10.0f, 5.0f };
	testObject_->GetRenderComponent()->SetObjectDrawType(ObjectDrawType::kTranslucent03);
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

	// タイルマップの生成(オブジェクト名、テクスチャパス、モデル、マップサイズX、マップサイズY、タイルサイズ、タイル間隔、マップデータ、テクスチャ名前<MapId>、マップの向き)
	object3dInstansManager_->CreateTileMap("test", "", GetDxCommon()->GetModelManager()->FindModel("stair.obj"), 10, 10, Vector3{ 1,1,3 }, { 10,20 }, map, mapId, MapAxis::XY);




	// スカイボックスの生成
	skyBox = std::make_unique<SkyBox>();
	skyBox->Initialize(GetEntity3DManager(), "resources/Texture/hdr/sky.dds");

	// 空
	sky_ = GetEntity3DManager()->CreateObject3D("skyBox", ObjectModelType::kSkyBox, {}, cameraManeger_->GetCamera());
	sky_->GetWorldTransform().scale_ = { 100,100,100 };
	sky_->SetSkyBox(skyBox.get());
};
//
void SampleScene::InitSprite() {

	sprite_ = std::make_unique<Sprite>();
	
	
	// スプライトの初期化
	sprite_->Initialize(GetEntity2DManager()->GetSpriteCommon(), "resources/Texture/uvChecker.png");
	sprite_->SetColor({1,1,1,1});			// 色
	sprite_->SetPosition({ 10,10 });		// 位置
	sprite_->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント
	
};


// 終了
void SampleScene::Finalize() {
	cameraManeger_->GetCamera()->Clear();
	cameraManeger_->Clear();

	fixedCamera_->GetUniqueCamera()->Clear();

};

// 毎フレーム更新
void SampleScene::Update() {

	// カメラ管理の更新
	cameraManeger_->Update();
	SetCamera(cameraManeger_->GetCamera());


	// インスタンスの検索
	ObjectInstans* obj = object3dInstansManager_->GetObjectById("test", 5);
	if (obj) {
		obj->transform.translate_.y += 0.01f; // 1マス持ち上げる
		obj->Update();
	}

	
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