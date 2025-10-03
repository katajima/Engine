#include"Framework.h"

void Framework::Initialize()
{
	// WindowsAPI解放
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();

	// Input
	input_ = std::make_unique<Input>();
	input_->Intialize(winApp.get());

	// グローバル
	globalVariables_ = std::make_unique<GlobalVariables>();
	
	// DirectX
	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Intialize(winApp.get());
	dxCommon->GetImGuiManager()->SetInput(input_.get());

	// 3D全般
	entity3DManager_ = std::make_unique<Entity3DManager>();
	entity3DManager_->Initialize(dxCommon.get());
	entity3DManager_->GetCameraCommon()->SetInput(input_.get());

	// 2D全般
	entity2DManager_ = std::make_unique<Entity2DManager>();
	entity2DManager_->Initialize(dxCommon.get());



}

void Framework::Finalize()
{
	// WindowsAPIの終了処理
	winApp->Finalize();
	
	// DirectX
	dxCommon->Finalize();

}

void Framework::Update()
{
	// Input
	input_->Update();

	
	// Windowsのメッセージ処理
	if(winApp->ProcessMessage()) {
		// ゲームループを抜ける
		endRequst_ = true;
	};
}

void Framework::Run()
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
