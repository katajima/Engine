#include "DepthStencil.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp.h"
#include "DirectXGame/engine/Manager/DSV/DsvManager.h"

void DepthStencil::Initialize(DXGIDevice* dxgi, Command* command, DsvManager* dsvManager)
{
	DXGIDevice_ = dxgi;
	command_ = command;
	dsvManager_ = dsvManager;

	CreateDepthStencilView();
}

void DepthStencil::ClearDepthView()
{
	

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture

	// DSVHeapの先頭にDSVを作る
	DXGIDevice_->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvManager_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil::GetCPUHandleDepthStencilResorce()
{
	return dsvManager_->GetCPUDescriptorHandle(dsvIndex_);
}

void DepthStencil::CreateDepthStencilView()
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::GetClientWidth();  // Textureの幅
	resourceDesc.Height = WinApp::GetClientHeight(); // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //　DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る


	//深度値のクリア
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる


	//Resourceの生成

	HRESULT hr = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特に無し
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最高値
		IID_PPV_ARGS(&depthStencilResource_)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	// インデックス割り当て
	dsvIndex_ = dsvManager_->Allocate();
	// dsv作成
	dsvManager_->CreateDSV(
		dsvIndex_,
		depthStencilResource_.Get(),
		DXGI_FORMAT_D24_UNORM_S8_UINT
	);
}
