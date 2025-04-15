#include "Command.h"

#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"

#include<cassert>

void Command::Initialize(DXGIDevice* dxgi)
{
	dxgi_ = dxgi;
	
	////------コマンドキュー------////

	//コマンドキューを生成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	hr_ = dxgi_->GetDevice()->CreateCommandQueue(&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));

	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));


	////------コマンドアロケータ------////

	//コマンドアロケータを生成する
	hr_ = dxgi_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));

	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));


	////------コマンドリスト------////

	//コマンドリストを生成する
	hr_ = dxgi_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));

	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

void Command::KickCommand()
{
	// コマンドリストを確定させる
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	// コマンドリストの実行
	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());
}

void Command::ResetCommand()
{
	// 次フレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));

	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

ID3D12CommandQueue* Command::GetQueue()
{
	return commandQueue_.Get();
}

ID3D12CommandAllocator* Command::GetAllocator()
{
	return commandAllocator_.Get();
}

ID3D12GraphicsCommandList* Command::GetList()
{
	return commandList_.Get();
}
