#include "Fence.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"

Engine::Fence::~Fence()
{
	// 終了処理漏れでもイベントハンドルを残さないようにする。
	Finalize();
}

void Engine::Fence::Initialize(DXGIDevice* dxgi, Command* command)
{
	this->dxgi = dxgi;		// デバイス
	this->command = command;	// コマンド

	CreateFence();	// フェンス生成
}

void Engine::Fence::Finalize()
{
	// CreateEventで作成した待機イベントを閉じ、Debug時のハンドルリークを防ぐ。
	if (fenceEvent_) {
		CloseHandle(fenceEvent_);
		fenceEvent_ = nullptr;
	}
}

void Engine::Fence::WaitGPU()
{
	// 初期化前や終了処理後に呼ばれた場合は何もしない。
	if (!command || !fence_ || !fenceEvent_) {
		return;
	}

	// フェンスの値を更新し、GPUとの同期
	fenceValue_++;
	//commandQueue->Signal(fence.Get(), fenceValue);
	command->GetQueue()->Signal(fence_.Get(), fenceValue_);
	// GPUの処理が完了するまで待機
	if (fence_->GetCompletedValue() < fenceValue_) {
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		WaitForSingleObject(fenceEvent_, INFINITE);
	}
}

void Engine::Fence::CreateFence()
{
	//初期値0でFenceを作る
	fenceValue_ = 0;

	hr_ = dxgi->GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));

	//FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}
