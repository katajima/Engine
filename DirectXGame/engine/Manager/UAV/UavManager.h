#pragma once

#include"DirectXGame/engine/base/DirectXCommon.h"
#include <cstdint> // 追加


// UAV管理
class UavManager {
public:
    // シングルトンインスタンス
    static UavManager* GetInstance();

    // 初期化
    void Initialize(DirectXCommon* dxCommon);

    void Finalize();

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
    static UavManager* instance;

    DirectXCommon* directXCommon_ = nullptr;

    // UAV用のデスクリプタサイズ
    uint32_t descriptorSize;
    // UAV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // 次に使用するUAVインデックス
    uint32_t useIndex = 0;
};
