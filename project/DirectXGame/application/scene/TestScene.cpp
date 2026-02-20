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


	AppGlobalVariables();
}

void TestScene::Finalize()
{

}

void TestScene::Update()
{
	SwitchRoom(); // 部屋切り替え
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManeger_->GetCamera());

	
#ifdef _DEBUG

	int count = 0;
	for (auto skin : skinObjects) {
		
		skin->GetAnimationComponent()->SetAnimationSpeed(1.0);
		skin->GetAnimationComponent()->SetIsLoop(true);
		count++;
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
	
	cameraManeger_->Update();

}

void TestScene::Draw3D()
{
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
		sprite_[1]->Draw(Engine::PSOType::NoUvInterpolation_MODE_SOLID_BACK);
		sprite_[2]->Draw(Engine::PSOType::UvInterpolation_MODE_WIREFRAME_BACK);
		sprite_[3]->Draw(Engine::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK);



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

void TestScene::AppGlobalVariables(){}


#pragma region Initialize

/// <summary>
/// 3Dオブジェクトの初期化
/// </summary>
void TestScene::InitializeObject3D()
{
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(cameraManeger_->GetCamera());


	ocean_ = std::make_unique<Engine::Ocean>();
	ocean_->Initialize(GetEntity3DManager(), { 10000,10000 });
	ocean_->GetMaterial()->GetMaterialInstance().enableLighting_ = false;


	oceanObject = GetEntity3DManager()->CreateObject3D("ocean", Engine::ObjectModelType::kOcean, {}, cameraManeger_->GetCamera());
	oceanObject->SetOcean(ocean_.get());
	oceanObject->GetWorldTransform().translate_ = { 0,-30,0 };
	oceanObject->GetWorldTransform().rotate_.x = Math::DegreesToRadians(90);
	oceanObject->GetRenderComponent()->SetObjectDrawType(Engine::ObjectDrawType::kTranslucent03);
	oceanObject->SetIsDraw(true);
	
	
	skyBox = std::make_unique<Engine::SkyBox>();
	skyBox->Initialize(GetEntity3DManager(), "resources/Texture/hdr/sky.dds");
	
	skyBox2 = std::make_unique<Engine::SkyBox>();
	skyBox2->Initialize(GetEntity3DManager(), "resources/Texture/hdr/sky.dds");


	skyBoxObject = GetEntity3DManager()->CreateObject3D("skyBox", Engine::ObjectModelType::kSkyBox, {}, cameraManeger_->GetCamera());
	skyBoxObject->SetSkyBox(skyBox.get());
	skyBoxObject->GetWorldTransform().scale_ = {10,10,10};
	skyBoxObject->SetIsDraw(true);
	
	
	skyBoxObject2 = GetEntity3DManager()->CreateObject3D("skyBox2", Engine::ObjectModelType::kSkyBox, {}, cameraManeger_->GetCamera());
	skyBoxObject2->SetSkyBox(skyBox2.get());
	skyBoxObject2->GetWorldTransform().scale_ = {1,1,1};
	skyBoxObject2->SetIsDraw(false);



	for(int i = 0; i < 1; i++)
	{
		auto obj = GetEntity3DManager()->CreateObject3D("skinObject_" + std::to_string(i), Engine::ObjectModelType::kSkinning, { static_cast<float>(i * 10),0,30 }, cameraManeger_->GetCamera());
		obj->SetModel("testCharacter.gltf");
		obj->InitAnimationComponent();
		obj->GetAnimationComponent()->SetAnimation("Idle01", static_cast<float>(i) / 10.0f );
		obj->SetIsDraw(true);
		skinObjects.push_back(obj);
	}
}

/// <summary>
/// スプライトの初期化
/// </summary>
void TestScene::InitializeObject2D()
{
	
	for (int i = 0; i < 4; i++)
	{
		auto sprite = std::make_unique<Engine::Sprite>();

		sprite->Initialize(GetEntity2DManager()->GetSpriteCommon(), "resources/Texture/uvChecker.png", false);
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
	GetEntity3DManager()->GetEffectManager()->GetParticleManager()->SetCamera(cameraManeger_->GetCamera());
	GetEntity3DManager()->GetEffectManager()->GetGpuParticleManager()->SetCamera(cameraManeger_->GetCamera());
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
	point = std::make_shared<Engine::PointLight>();

	point->point = pointLightData;

	//GetEntity3DManager()->GetLightManager()->AddLight(point);

	SpotLightData spotLightData;

	spotLightData = SpotLightData({ 1.0f,1.0f,1.0f,1.0f }, { 2.0f,1.25f,0.0f }, 10.0f, Normalize({ -1.0f,-1.0f,0.0f }), 10.0f, 2.0f, std::cos(std::numbers::pi_v<float> / 3.0f), 1.0f);
	spotLightData.position = Vector3(10, 0, 10);
	spotLightData.intensity = 50.0f;
	spotLightData.isLight = true;
	spotLightData.decay = 1.0f;

	spot = std::make_shared<Engine::SpotLight>();

	spot->spot = spotLightData;
	//GetEntity3DManager()->GetLightManager()->AddLight(spot);

	GetEntity3DManager()->Get3DLineCommon()->SetDefaltCamera(cameraManeger_->GetCamera());

	SetCamera(cameraManeger_->GetCamera());
	
	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 0.9f;
	directionalLightData.isLight = true;



	directional = std::make_shared<Engine::DirectionalLight>();
	directional->directional = directionalLightData;

	GetEntity3DManager()->GetLightManager()->AddLight(directional);

}

/// <summary>
/// カメラの初期化 
/// </summary>
void TestScene::InitializeCamera()
{
	// 入力システム初期化
	inputSystem_ = std::make_unique<InputSystem>();
	inputSystem_->Initialize(GetInput());


	// 固定カメラ
	fixedCamera_ = std::make_unique<FixedCamera>();
	fixedCamera_->Initialize(inputSystem_.get(), GetEntity3DManager(),  GetGlobalVariables(), {});
	
	// カメラ管理
	cameraManeger_ = std::make_unique<CameraManager>();
	cameraManeger_->Initialize(inputSystem_.get(), GetEntity3DManager(),GetGlobalVariables());
	// カメラ追加
	cameraManeger_->AddCamera({ fixedCamera_.get(),false }, "fixedCamera");
	cameraManeger_->SetUseCamera("fixedCamera", 0.0f);
}

/// <summary>
/// その他初期化
/// </summary>
void TestScene::InitializeOthers()
{

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
	
}

void TestScene::UpdateRoom02()
{
	if (input_->IsTriggerKey(DIK_P)) {
		GetSceneManager()->ChangeScene("GAMEPLAY");
	}

	//emitter_->Update();

}

void TestScene::UpdateRoom03()
{
	
}

void TestScene::UpdateRoom04()
{

	
}

void TestScene::UpdateRoom05()
{
	GetEntity3DManager()->Get3DLineCommon()->GetDebugLineMeshData().AddGrid(1000, 1000, 10, { 1,1,1,1 });
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

