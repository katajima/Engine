#include"Framework.h"

void Engine::Framework::Initialize()
{
	// WindowsAPI解放
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	// Input
	input_ = std::make_unique<Input>();
	input_->Intialize(winApp_.get());

	// 音
	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialize();


	// グローバル
	globalVariables_ = std::make_unique<GlobalVariables>();
	
	// DirectX
	dxCommon_ = std::make_unique<Engine::DirectXCommon>();
	dxCommon_->Intialize(winApp_.get());
	dxCommon_->GetImGuiManager()->SetInput(input_.get());

	// 3D全般
	entityManager_ = std::make_unique<EntityManager>();
	entityManager_->Initialize(dxCommon_.get());
	// コンボや各ゲームオブジェクトから共通の音声管理へアクセスできるよう関連付ける。
	entityManager_->SetAudioManager(audioManager_.get());
	entityManager_->GetCameraCommon()->SetInput(input_.get());
}

void Engine::Framework::Finalize()
{
	// 最後の描画で使ったシーン/エンティティのGPUリソースを安全に破棄するため、先にGPU完了を待つ。
	if (dxCommon_ && dxCommon_->GetFence()) {
		dxCommon_->GetFence()->WaitGPU();
	}

	// シーンはDirectXリソースを持つため、DirectXCommonを破棄する前に終了させる。
	sceneManager_.reset();

	// シーン生成用ファクトリはシーン破棄後に不要になるため解放する。
	sceneFactory_.reset();

	// エンティティ群が持つGPUリソースを、DirectXCommonより先に解放する。
	entityManager_.reset();

	// DirectX
	if (dxCommon_) {
		WinApp::SetSwapChain(nullptr);
		dxCommon_->Finalize();
		dxCommon_.reset();
	}

	// 音
	if (audioManager_) {
		audioManager_->Finalize();
		audioManager_.reset();
	}

	// グローバル変数
	globalVariables_.reset();

	// 入力
	input_.reset();

	// WindowsAPIの終了処理
	if (winApp_) {
		winApp_->Finalize();
		winApp_.reset();
	}

}

void Engine::Framework::Update()
{
	// 再生が完了した音声Voiceを毎フレーム回収する。
	audioManager_->Update();

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
