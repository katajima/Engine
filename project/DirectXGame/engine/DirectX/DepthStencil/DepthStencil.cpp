#include "DepthStencil.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"
#include "DirectXGame/engine/Manager/DSV/DsvManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"

void Engine::DepthStencil::Initialize(DXGIDevice* dxgi, Command* command, DsvManager* dsvManager, SrvManager* srvManager)
{
	dxgiDevice = dxgi;			// デバイス
	this->command = command;			// コマンド
	this->dsvManager = dsvManager;	// DSV管理クラス
	this->srvManager = srvManager;	// SRV管理クラス
	CreateDepthStencilView();	// デプスステンシルビュー生成
}

void Engine::DepthStencil::ClearDepthView()
{
	

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture

	// DSVHeapの先頭にDSVを作る
	dxgiDevice->GetDevice()->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvManager->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::DepthStencil::GetCPUHandleDepthStencilResorce()
{
	return dsvManager->GetCPUDescriptorHandle(dsvIndex_);
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::DepthStencil::GetCPUHandleDepthSRV()
{
	return srvManager->GetCPUDescriptorHandle(srvIndex_);
}

uint32_t Engine::DepthStencil::GetDepthSrvIndex() const
{
	return srvIndex_;
}

ID3D12Resource* Engine::DepthStencil::GetResource()
{
	return depthStencilResource_.Get();
}

void Engine::DepthStencil::CreateDepthStencilView()
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

	HRESULT hr = dxgiDevice->GetDevice()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特に無し
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最高値
		IID_PPV_ARGS(&depthStencilResource_)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

	// インデックス割り当て
	dsvIndex_ = dsvManager->Allocate();
	// dsv作成
	dsvManager->CreateDSV(
		dsvIndex_,
		depthStencilResource_.Get(),
		DXGI_FORMAT_D24_UNORM_S8_UINT
	);

	srvIndex_ = srvManager->Allocate();
	srvIndex_ = srvManager->Allocate();

	// SRVを作成
	DirectX::TexMetadata matadata{};
	matadata.format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	matadata.mipLevels = 1;
	srvManager->CreateSRVforTexture2D(srvIndex_, depthStencilResource_.Get(), matadata);

}


D3D12_GPU_DESCRIPTOR_HANDLE Engine::DepthStencil::GetSRVGPUHandle() {
	return srvManager->GetGPUDescriptorHandle(srvIndex_);
}