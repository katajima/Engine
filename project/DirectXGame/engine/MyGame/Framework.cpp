#include"Framework.h"

void Engine::Framework::Initialize()
{
	// WindowsAPI解放
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	// Input
	input_ = std::make_unique<Input>();
	input_->Intialize(winApp_.get());

	// グローバル
	globalVariables_ = std::make_unique<GlobalVariables>();
	
	// DirectX
	dxCommon_ = std::make_unique<Engine::DirectXCommon>();
	dxCommon_->Intialize(winApp_.get());
	dxCommon_->GetImGuiManager()->SetInput(input_.get());

	// 3D全般
	entityManager_ = std::make_unique<EntityManager>();
	entityManager_->Initialize(dxCommon_.get());
	entityManager_->GetCameraCommon()->SetInput(input_.get());
}

void Engine::Framework::Finalize()
{
	// WindowsAPIの終了処理
	winApp_->Finalize();
	
	// DirectX
	dxCommon_->Finalize();

}

void Engine::Framework::Update()
{
	// Input
	input_->Update();

	
	// Windowsのメッセージ処理
	if(winApp_->ProcessMessage()) {
		// ゲームループを抜ける
		endRequst_ = true;
	};
}

void Engine::Framework::Run()
{
	// ゲームの初期化
	Initialize();
	while (true) {
		// 毎フレーム更新
		Update();
		// 終了リクエストが来たら抜ける
		if (IsEndRequst()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}
