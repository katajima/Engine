#include"TitleScene.h"



void TitleScene::Initialize()
{
	//オーディオの初期化
	//audio_ = Audio::GetInstance();
	// 入力初期化
	input_ = GetInput();

	// カメラ
	InitializeCamera();
	// リソース
	InitializeResources();


	GetSceneData().playerID = 1;
}

void TitleScene::Finalize()
{
}

void TitleScene::Update()
{
	if (input_->IsControllerConnected()) {
		if (input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B)) {
			GetSceneManager()->ChangeScene("GAMEPLAY");
		}
	}


#ifdef _DEBUG
	if (!initialized) {
		attackSeq.events.push_back({ 5, 15, 0xFF00FF00, "Startup" });
		attackSeq.events.push_back({ 15, 25, 0xFFFFFF00, "Active" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		attackSeq.events.push_back({ 25, 45, 0xFFFF0000, "Recovery" });
		initialized = true;
	}

	ImGui::Begin("Attack Editor");



	currentFrame++;

	if (currentFrame >= attackSeq.GetFrameMax()) {
		currentFrame = 0;
	}

	ImSequencer::Sequencer(
		&attackSeq,
		&currentFrame,
		&expanded,
		&selected,
		&firstFrame,
		ImSequencer::SEQUENCER_EDIT_ALL |
		ImSequencer::SEQUENCER_ADD |
		ImSequencer::SEQUENCER_DEL
	);

	ImGui::Text("Current Frame: %d", currentFrame);
	ImGui::SliderFloat("Frame PixelWidthTarget",
		&ImSequencer::g_framePixelWidthTarget,
		1.0f, 40.0f, "%.1f px");
	ImGui::SliderFloat("Frame PixelWidth",
		&ImSequencer::g_framePixelWidth,
		1.0f, 40.0f, "%.1f px");
	ImGui::SliderInt("Frame LegendWidth",
		&ImSequencer::g_legendWidth,
		50, 500, "%d px");
	if (selected != -1) {
		ImGui::Text("Selected Event: %s", attackSeq.events[selected].name.c_str());
	}

	ImGui::End();
#endif // _DEBUG

	tail.Update();
	camera->UpdateMatrix();
}

void TitleScene::Draw3D()
{
	
	tail.Draw();
}

void TitleScene::Draw2D()
{
	icon_B->Update();
	icon_B->Draw();
	title->Update();
	title->Draw();
}

void TitleScene::InitializeResources()
{
	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());

	icon_B = std::make_unique<Sprite>();
	icon_B->Initialize(GetEntity2DManager()->GetSpriteCommon(),"resources/Texture/icon/B.png");
	icon_B->SetPosition({ 640,500 });
	icon_B->SetAnchorPoint({ 0.5f,0.5f });
	icon_B->SetSize({200,200});

	title = std::make_unique<Sprite>();
	title->Initialize(GetEntity2DManager()->GetSpriteCommon(),"resources/Texture/text/title.png");
	title->SetPosition({ 200,200 });
	//title->SetAnchorPoint({ 0.5f,0.5f });
	title->SetSize(2);


	tail.Initialize(GetEntity3DManager());
	tail.SetModel("renga.gltf");
	tail.SetCamera(camera.get());
	tail.GetWorldTransform().scale_ = { 10,10,10 };

	DirectionalLightData directionalLightData{};
	directionalLightData.color = { 1,1,1,1 };
	directionalLightData.direction = { 0,-1,0 };
	directionalLightData.intensity = 1.5f;
	directionalLightData.isLight = true;
	directionalLightData.lig = 0.1f;


	directional = std::make_shared<DirectionalLight>();
	directional->directional = directionalLightData;

	GetEntity3DManager()->GetLightManager()->AddLight(directional);
}

void TitleScene::InitializeCamera()
{
	camera = std::make_unique <Camera>();
	camera->Initialize(GetEntity3DManager()->GetCameraCommon());
	camera->transform_.rotate = { 1.0f,0,0 };
	camera->transform_.translate = { 0,100,-60.0f };
}
