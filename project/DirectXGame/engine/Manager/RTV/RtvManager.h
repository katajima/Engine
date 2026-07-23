#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


#include"DirectXGame/engine/struct/Structs3D.h"


namespace Engine {
    // 前方宣言
    class DXGIDevice;
    class Command;

    /// <summary>
    /// RTVマネージャークラス
    /// </summary>
    class RtvManager {
    public:
        /// <summary>
        /// 初期化
        /// </summary>
        void Initialize(DXGIDevice* DXGI, Command* Command);

        /// <summary>
        /// アロケート
        /// </summary>
        uint32_t Allocate();

        /// <summary>
        /// デクリメント
        /// </summary>
        void DecAllocate();

        /// <summary>
        /// デスクリプタハンドル計算
        /// </summary>
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
        /// <summary>
        /// デスクリプタハンドル計算
        /// </summary>
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

        /// <summary>
        /// RTV生成関数
        /// </summary>
        void CreateRTV(uint32_t rtvIndex, ID3D12Resource* pResource);
        /// <summary>
        /// レンダーテクスチャリソース生成
        /// </summary>
        Microsoft::WRL::ComPtr < ID3D12Resource> CreateRenderTextureResource(DXGI_FORMAT format, const Vector4& color);

        /// <summary>
        /// 終了
        /// </summary>
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
        DXGIDevice* dxgiDevice = nullptr;
        Command* command = nullptr;

    };
}
