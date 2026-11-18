#include"Framework.h"
#include "DirectXGame/engine/Base/Profiler/ScopedCpuTimer.h"

void Engine::Framework::Initialize()
{
	// WindowsAPI解放
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize();

	// Input
	input_ = std::make_unique<Input>();
	input_->Initialize(winApp_.get());

	// 音
	audioManager_ = std::make_unique<AudioManager>();
	audioManager_->Initialize();


	// グローバル
	globalVariables_ = std::make_unique<GlobalVariables>();

	// DirectX
	dxCommon_ = std::make_unique<Engine::DirectXCommon>();
	dxCommon_->Initialize(winApp_.get());
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
	if (sceneManager_) {
		sceneManager_->Finalize();
		sceneManager_.reset();
	}

	// シーン生成用ファクトリはシーン破棄後に不要になるため解放する。
	sceneFactory_.reset();

	// エンティティ群が持つGPUリソースを、DirectXCommonより先に解放する。
	if (entityManager_) {
		entityManager_->Finalize();
		entityManager_.reset();
	}

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

void Engine::Framework::Update(float deltaTime)
{
	// 基盤更新全体を計測し、音声・入力・メッセージ処理の負荷を確認できるようにする。
	ENGINE_CPU_PROFILE_SCOPE("Framework::Update");

	// 再生が完了した音声Voiceを毎フレーム回収する。
	audioManager_->Update();

	// Input
	input_->Update();


	// Windowsのメッセージ処理
	if(winApp_->ProcessMessage()) {
		// ゲームループを抜ける
		endRequest_ = true;
	};
}

void Engine::Framework::Run()
{
	// ゲームの初期化
	Initialize();
	auto previousTime = std::chrono::steady_clock::now();
	while (true) {
		// UpdateとDrawを含む1ループを1フレームとして記録する。
		CpuProfiler::Get().BeginFrame();
		{
			// フレーム全体の時間を、UIの階層表示で最上位に表示する。
			ENGINE_CPU_PROFILE_SCOPE("Frame");

		// 毎フレーム更新
			const auto currentTime = std::chrono::steady_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
			previousTime = currentTime;
			Update(deltaTime);
			// 終了リクエストが来たら描画をスキップしてフレームを閉じる。
			if (!IsEndRequest()) {
				// 描画
				Draw();
			}
		}
		// スコープの破棄後にフレームを確定し、次フレームのUIで参照できるようにする。
		CpuProfiler::Get().EndFrame();
		if (IsEndRequest()) {
			break;
		}
	}
	// ゲームの終了
	Finalize();
}
