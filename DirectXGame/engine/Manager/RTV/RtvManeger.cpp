#include "RtvManeger.h"
#include "assert.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/base/WinApp.h"

const uint32_t RtvManager::kMaxRTVCount = 16;



void RtvManager::Initialize(DXGIDevice* DXGI, Command* Command)
{
	DXGIDevice_ = DXGI;
	command_ = Command;

	// デスクリプタヒープ
	descriptorHeap = DXGIDevice_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kMaxRTVCount, false);
	// デスクリプタ一個分のサイズを取得して記録
	descriptorSize = DXGIDevice_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}



uint32_t RtvManager::Allocate()
{
	assert(kMaxRTVCount > useIndex);

	// return する番号を一旦記録
	int index = useIndex;
	// 次回のために番号を1進める
	useIndex++;
	// 上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE RtvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize;
	return handle;
}
D3D12_GPU_DESCRIPTOR_HANDLE RtvManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * descriptorSize;
	return handle;
}


void RtvManager::CreateRTV(uint32_t rtvIndex, ID3D12Resource* pResource) { 
	assert(pResource && "Resource is nullptr!");

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUDescriptorHandle(rtvIndex);
	DXGIDevice_->GetDevice()->CreateRenderTargetView(pResource, &rtvDesc, handle);
}

Microsoft::WRL::ComPtr<ID3D12Resource> RtvManager::CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& color)
{
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::GetClientWidth();  // Textureの幅
	resourceDesc.Height = WinApp::GetClientHeight(); // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = format; //　RenderTargertとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして使う通知
	//resourceDesc.Flags = D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE; // RenderTargetとして使う通知

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 当然VRAM上に作る

	// ClearValue
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = color.x;
	clearValue.Color[1] = color.y;
	clearValue.Color[2] = color.z;
	clearValue.Color[3] = color.w;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;

	HRESULT hr = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		&clearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

