#include "Fence.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"

void Engine::Fence::Initialize(DXGIDevice* dxgi, Command* command)
{
	this->dxgi = dxgi;		// デバイス
	this->command = command;	// コマンド

	CreateFence();	// フェンス生成
}

void Engine::Fence::WaitGPU()
{
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
