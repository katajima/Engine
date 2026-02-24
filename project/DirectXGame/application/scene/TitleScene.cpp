#include"TitleScene.h"



void TitleScene::Initialize()
{
	// 入力初期化
	input_ = GetInput();
	// カメラ
	InitializeCamera();
	// リソース
	InitializeResources();
	// 
	GetSceneData().playerID = 1;

	// ステージ
	titleStage_ = std::make_unique<TitleStage>();
	titleStage_->Initialize(GetDxCommon(), GetEntity3DManager(), GetEntity2DManager(), camera.get());

	// UI
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(nullptr, GetEntity2DManager(), GetGlobalVariables());

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

	// ステージ更新
	titleStage_->Update();
	// UI更新
	titleUI_->Update(GetTime());
	// カメラ更新
	camera->UpdateMatrix();
}

void TitleScene::Draw3D(){}

void TitleScene::Draw2D(){
	// UI更新
	titleUI_->Draw();
}

void TitleScene::InitializeResources()
{
	// オブジェクト3D
	GetEntity3DManager()->GetObject3dCommon()->SetDefaltCamera(camera.get());
}

void TitleScene::InitializeCamera()
{
	camera = std::make_unique <Engine::Camera>();
	camera->Initialize(GetEntity3DManager()->GetCameraCommon());
	//camera->transform_.rotate = { 1.0f,0,0 };
	camera->transform_.translate = { 0,100,-60.0f };
}
