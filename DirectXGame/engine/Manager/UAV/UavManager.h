#pragma once


// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>

#include"DirectXGame/engine/struct/Structs3D.h"

class DXGIDevice;
class Command;

// UAV管理
class UavManager {
public:
    
    // 初期化
    void Initialize(DXGIDevice* DXGI, Command* Command);

    
    uint32_t Allocate();

    // デスクリプタハンドル計算
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    // UAV生成関数
    // UAV生成(Structured Buffer用)
    void CreateUAVforStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
    // UAV生成(テクスチャ用)
    void CreateUAVforTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT format);

    void PreDraw();

    // UAVセットコマンド
    void SetComputeRootDescriptorTable(UINT RootParameterIndex, uint32_t uavIndex);

public:
    // 最大UAV数
    static const uint32_t kMaxUAVCount;
private:
    // UAV用のデスクリプタサイズ
    uint32_t descriptorSize;
    // UAV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // 次に使用するUAVインデックス
    uint32_t useIndex = 0;
private:
    DXGIDevice* DXGIDevice_;
    Command* command_;
};
