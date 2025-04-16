#include "SwapChain.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"

void SwapChain::Initialize(WinApp* winApp, DXGIDevice* dxgi, Command* command, RtvManager* rtvManager)
{
	winApp_ = winApp;
	DXGIDevice_ = dxgi;
	command_ = command;
	rtvManager_ = rtvManager;


	// スワップチェーン作成
	CreateSwapChain();
	// スワップチェーンリソースの作成
	CreateSwapChainResource();
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

void SwapChain::CreateSwapChain()
{
	////------スワップチェーン------////
	//WinApp::GetClientWidth;
	//スワップチェーンを生成する
	swapChainDesc_.Width = WinApp::GetClientWidth();   //画面の幅。ウィンドウクライアント領域を同じものにしておく
	swapChainDesc_.Height = WinApp::GetClientHeight();  //画面の高さ。ウィンドウクライアント領域を同じものにしておく
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

	
	
	rtvIndex_[0] = rtvManager_->Allocate();
	rtvManager_->CreateRTV(rtvIndex_[0], swapChainResources_[0].Get());

	rtvIndex_[1] = rtvManager_->Allocate();
	rtvManager_->CreateRTV(rtvIndex_[1], swapChainResources_[1].Get());
}
