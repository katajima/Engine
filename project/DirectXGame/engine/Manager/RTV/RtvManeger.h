#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


#include"DirectXGame/engine/struct/Structs3D.h"

// 前方宣言
class DXGIDevice;
class Command;

/// <summary>
/// RTVマネージャークラス
/// </summary>
class RtvManager {
public:
    // 初期化
    void Initialize(DXGIDevice* DXGI, Command* Command);

    // アロケート
    uint32_t Allocate();

    // デクリメント
    void DecAllocate();

    // デスクリプタハンドル計算
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
    // デスクリプタハンドル計算
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

    // RTV生成関数
    void CreateRTV(uint32_t rtvIndex, ID3D12Resource* pResource);
    // レンダーテクスチャリソース生成
    Microsoft::WRL::ComPtr < ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& color);

    // 終了
    void Finalize();

public:
    // 最大RTV数
    static const uint32_t kMaxRTVCount;



private:
    // RTV用のデスクリプタサイズ
    uint32_t descriptorSize;
    // RTV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // 次に使用するRTVインデックス
    uint32_t useIndex = 0;
private:
    DXGIDevice* DXGIDevice_;
    Command* command_;

};

