#include "DsvManager.h"

#include "assert.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp.h"

const uint32_t DsvManager::kMaxDsvCount = 1;

void DsvManager::Initialize(DXGIDevice* dxgi, Command* command)
{
	DXGIDevice_ = dxgi;
	command_ = command;
	// ディスクリプタヒープの作成
	descriptorHeap_ = DXGIDevice_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, kMaxDsvCount, false);
	// デスクリプタ一個分のサイズを取得して記録
	descriptorSize_ = DXGIDevice_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

uint32_t DsvManager::Allocate()
{
	assert(kMaxDsvCount > useIndex_);

	// return する番号を一旦記録
	int index = useIndex_;
	// 次回のために番号を1進める
	useIndex_++;
	// 上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE DsvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DsvManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize_;
	return handle;
}

void DsvManager::CreateDSV(uint32_t dsvIndex, ID3D12Resource* pResource, DXGI_FORMAT format)
{
	// dsvの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;



	// dsv作成
	DXGIDevice_->GetDevice()->CreateDepthStencilView(pResource, &dsvDesc, GetCPUDescriptorHandle(dsvIndex));
}

ID3D12DescriptorHeap* DsvManager::GetDescriptorHeap() const
{
	return descriptorHeap_.Get();
}

uint32_t DsvManager::GetDescriptorSize() const
{
	return descriptorSize_;
}
