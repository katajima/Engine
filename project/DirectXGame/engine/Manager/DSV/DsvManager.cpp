#include "DsvManager.h"

#include "assert.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

const uint32_t Engine::DsvManager::kMaxDsvCount = 1;

void Engine::DsvManager::Initialize(DXGIDevice* dxgi, Command* command)
{
	this->dxgiDevice = dxgi;	// デバイス
	this->command = command;	// コマンド
	// ディスクリプタヒープの作成
	descriptorHeap_ = dxgiDevice->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxDsvCount, false);
	// デスクリプタ一個分のサイズを取得して記録
	descriptorSize_ = dxgiDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

uint32_t Engine::DsvManager::Allocate()
{
	assert(kMaxDsvCount > useIndex_);

	// return する番号を一旦記録
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::DsvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE Engine::DsvManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}

void Engine::DsvManager::CreateDSV(uint32_t dsvIndex, ID3D12Resource* pResource, DXGI_FORMAT format)
{
	// dsvの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;



	// dsv作成
	dxgiDevice->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, GetCPUDescriptorHandle(dsvIndex));
}

ID3D12DescriptorHeap* Engine::DsvManager::GetDescriptorHeap() const
{
	return descriptorHeap_.Get();
}

uint32_t Engine::DsvManager::GetDescriptorSize() const
{
	return descriptorSize_;
}
