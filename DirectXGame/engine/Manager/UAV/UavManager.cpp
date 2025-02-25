#include "UavManager.h"

const uint32_t UavManager::kMaxUAVCount = 512;

UavManager* UavManager::instance = nullptr;

UavManager* UavManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new UavManager;
    }
    return instance;
}

void UavManager::Initialize(DirectXCommon* dxCommon)
{
    this->directXCommon_ = dxCommon;

    // デスクリプタヒープ
    descriptorHeap = directXCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxUAVCount, true);
    // デスクリプタ一個分のサイズを取得して記録
    descriptorSize = directXCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void UavManager::Finalize()
{
    descriptorHeap.Reset();
    delete instance;
    instance = nullptr;
}

uint32_t UavManager::Allocate()
{
    assert(kMaxUAVCount > useIndex);

    int index = useIndex;
    useIndex++;
    return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE UavManager::GetCPUDescriptorHandle(uint32_t index)
{
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE UavManager::GetGPUDescriptorHandle(uint32_t index)
{
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

void UavManager::CreateUAVforStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = numElements;
    uavDesc.Buffer.StructureByteStride = structureByteStride;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    directXCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}

void UavManager::CreateUAVforTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT format)
{
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;
    uavDesc.Texture2D.PlaneSlice = 0;

    directXCommon_->GetDevice()->CreateUnorderedAccessView(pResource, nullptr, &uavDesc, GetCPUDescriptorHandle(uavIndex));
}

void UavManager::PreDraw()
{
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
    directXCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
}

void UavManager::SetComputeRootDescriptorTable(UINT RootParameterIndex, uint32_t uavIndex)
{
    directXCommon_->GetCommandList()->SetComputeRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(uavIndex));
}