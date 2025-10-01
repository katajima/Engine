#include "SwapChain.h"

#include "DirectXGame/engine/base/WinApp/WinApp.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"
#include "DirectXGame/engine/DirectX/Fence/Fence.h"

void SwapChain::Initialize(WinApp* winApp, DXGIDevice* dxgi, Command* command, RtvManager* rtvManager, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort, Fence* fence)
{
	winApp_ = winApp;
	DXGIDevice_ = dxgi;
	command_ = command;
	rtvManager_ = rtvManager;
	barrier_ = barrier;
	scissorRect_ = scissorRect;
	viewPort_ = viewPort;
	fence_ = fence;
	// スワップチェーン作成
	CreateSwapChain();
	// スワップチェーンリソースの作成
	CreateSwapChainResource();

	rtvIndex_[0] = rtvManager_->Allocate();
	rtvIndex_[1] = rtvManager_->Allocate();
	// RTV作成
	CreateRTV();

	swapChainResources_[0]->SetName(L"SwapChainBuffer_0");
	swapChainResources_[1]->SetName(L"SwapChainBuffer_1");

}

void SwapChain::Present()
{
	// GPUに画面交換を通知
	swapChain_->Present(1, 0);
}

ID3D12Resource* SwapChain::GetCurrentBackBufferResource()
{
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	return swapChainResources_[backBufferIndex_].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentBackBufferRTVHandle()
{
	backBufferIndex_ = swapChain_->GetCurrentBackBufferIndex();
	return rtvManager_->GetCPUDescriptorHandle(rtvIndex_[backBufferIndex_]);
}

void SwapChain::PreDraw()
{
	// スワップチェーン用
	barrier_->TransitionResource(GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// 描画先のRTVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentBackBufferRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };  // 任意のクリアカラー（青）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// コマンドを積む
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void SwapChain::PostDraw()
{
	// スワップチェーン用
	barrier_->TransitionResource(GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドキック
	command_->KickCommand();

	// GPUに画面交換を通知
	Present();

	// フェンス
	fence_->WaitGPU();

	// コマンドリセット
	command_->ResetCommand();
}

void SwapChain::Resize(int width, int height)
{
	if (!swapChain_) return;

	// GPUの使用完了を待つ（安全のため）
	fence_->WaitGPU();

	// バックバッファを解放
	for (auto& res : swapChainResources_) {
		res.Reset();
	}

	// バッファをリサイズ
	hr_ = swapChain_->ResizeBuffers(
		2, width, height,
		DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	assert(SUCCEEDED(hr_));

	// 新しいリソースを再取得
	CreateSwapChainResource();

	// RTVも作り直す
	CreateRTV();
}

void SwapChain::CreateSwapChain()
{
	////------スワップチェーン------////
	//スワップチェーンを生成する
	int32_t width = WinApp::GetClientWidth(false);
	int32_t height = WinApp::GetClientHeight(false);

#ifndef _DEBUG
	//width = WinApp::GetClientWidth();
	//height = WinApp::GetClientHeight();
#endif // !_DEBUG




	swapChainDesc_.Width = width;   //画面の幅。ウィンドウクライアント領域を同じものにしておく
	swapChainDesc_.Height = height;  //画面の高さ。ウィンドウクライアント領域を同じものにしておく
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  //色の形式
	swapChainDesc_.SampleDesc.Count = 1;  //マルチサンプルしない
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;  //描画ターゲットとして利用する
	swapChainDesc_.BufferCount = 2;  //ダブルバッファ
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;  //モニターにうつしたら、中身を確認

	//コマンドキュー、ウィンドウハンドル
	hr_ = DXGIDevice_->GetFactory()->CreateSwapChainForHwnd(command_->GetQueue(), WinApp::GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr_));
}

void SwapChain::CreateSwapChainResource()
{

	// SwapChainからResourceを取得
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	assert(SUCCEEDED(hr_)); // 成功を確認

	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(hr_));

	swcDesc_ = {};
	hr_ = swapChain_->GetDesc(&swcDesc_);
	assert(SUCCEEDED(hr_));
}

void SwapChain::CreateRTV()
{
	// バックバッファの数に応じてサイズを調整
	backBuffers_.resize(swcDesc_.BufferCount);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//rtvTexDesc_ = rtvDesc;




	rtvManager_->CreateRTV(rtvIndex_[0], swapChainResources_[0].Get());

	rtvManager_->CreateRTV(rtvIndex_[1], swapChainResources_[1].Get());
}
