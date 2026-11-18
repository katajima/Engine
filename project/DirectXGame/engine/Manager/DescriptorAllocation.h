#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>
#include <memory>

namespace Engine {
namespace Detail {
    struct DescriptorLease;
    struct DescriptorPool;
}
/// @brief デスクリプタ領域と関連リソースの所有権を共有する。
class DescriptorAllocation {
public:
    /// @brief ヒープ内の割り当て位置を取得する。
    /// @return 所有中のインデックス。
    uint32_t GetIndex() const;
    /// @brief リソースをデスクリプタと同じGPU完了時点まで保持する。
    /// @param resource 保持するリソース。
    void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
private:
    friend class DescriptorAllocator;
    std::shared_ptr<Detail::DescriptorLease> mLease;
};
/// @brief GPU完了通知後に解放領域を再利用する。
class DescriptorAllocator {
public:
    /// @brief ヒープの容量と先頭予約数を指定する。
    /// @param capacity ヒープ全体の容量。
    /// @param reservedCount 外部ライブラリなどの予約数。
    explicit DescriptorAllocator(uint32_t capacity, uint32_t reservedCount = 0);
    /// @brief 未使用領域の所有権を取得する。
    /// @return 割り当てた領域。
    DescriptorAllocation Allocate();
    /// @brief 記録済みコマンドのGPU完了後に保留領域を回収する。
    void CollectRetired();
    /// @brief 一時リソースを次のGPU完了通知まで保持する。
    /// @param resource コピー元や再生成前のリソース。
    void RetainUntilGpuComplete(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
    /// @brief 新たに割り当て可能か確認する。
    /// @return 空き領域がある場合はtrue。
    bool HasCapacity() const;
private:
    std::shared_ptr<Detail::DescriptorPool> mPool;
};
}
