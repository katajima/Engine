#include "DescriptorAllocation.h"

#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Engine {
    namespace Detail {
        struct RetiredDescriptor {
            uint32_t mIndex = UINT32_MAX;
            std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> mResources;
            std::unique_ptr<RetiredDescriptor> mNext;
        };
        struct DescriptorPool {
            std::mutex mMutex;
            uint32_t mNext = 0;
            uint32_t mCapacity = 0;
            std::vector<uint32_t> mFreeIndices;
            std::unique_ptr<RetiredDescriptor> mRetired;
        };
        struct DescriptorLease {
            std::weak_ptr<DescriptorPool> mPool;
            std::unique_ptr<RetiredDescriptor> mRetirement;
            /// @brief 最後の所有者が破棄された領域とリソースをGPU完了待ちへ移す。
            ~DescriptorLease() {
                if (auto owner = mPool.lock()) {
                    std::scoped_lock lock(owner->mMutex);
                    mRetirement->mNext = std::move(owner->mRetired);
                    owner->mRetired = std::move(mRetirement);
                }
            }
        };
    }
}

uint32_t Engine::DescriptorAllocation::GetIndex() const { if (!mLease) throw std::logic_error("Empty descriptor allocation"); return mLease->mRetirement->mIndex; }

void Engine::DescriptorAllocation::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    if (!mLease) throw std::logic_error("Empty descriptor allocation");
    mLease->mRetirement->mResources.push_back(std::move(resource));
}

Engine::DescriptorAllocator::DescriptorAllocator(uint32_t capacity, uint32_t reservedCount) : mPool(std::make_shared<Detail::DescriptorPool>()) {
    if (reservedCount > capacity) throw std::invalid_argument("Descriptor reservation exceeds capacity");
    mPool->mNext = reservedCount;
    mPool->mCapacity = capacity;
    mPool->mFreeIndices.reserve(capacity);
}

Engine::DescriptorAllocation Engine::DescriptorAllocator::Allocate() {
    auto lease = std::make_shared<Detail::DescriptorLease>();
    lease->mRetirement = std::make_unique<Detail::RetiredDescriptor>();
    std::scoped_lock lock(mPool->mMutex);
    uint32_t index;
    if (!mPool->mFreeIndices.empty()) { index = mPool->mFreeIndices.back(); mPool->mFreeIndices.pop_back(); }
    else {
        if (mPool->mNext >= mPool->mCapacity) throw std::overflow_error("Descriptor heap exhausted");
        index = mPool->mNext++;
    }
    DescriptorAllocation allocation;
    allocation.mLease = std::move(lease);
    allocation.mLease->mPool = mPool;
    allocation.mLease->mRetirement->mIndex = index;
    return allocation;
}

void Engine::DescriptorAllocator::CollectRetired() {
    std::scoped_lock lock(mPool->mMutex);
    while (mPool->mRetired) {
        auto entry = std::move(mPool->mRetired);
        mPool->mRetired = std::move(entry->mNext);
        if (entry->mIndex != UINT32_MAX) mPool->mFreeIndices.push_back(entry->mIndex);
    }
}

void Engine::DescriptorAllocator::RetainUntilGpuComplete(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    auto entry = std::make_unique<Detail::RetiredDescriptor>();
    entry->mResources.push_back(std::move(resource));
    std::scoped_lock lock(mPool->mMutex);
    entry->mNext = std::move(mPool->mRetired);
    mPool->mRetired = std::move(entry);
}

bool Engine::DescriptorAllocator::HasCapacity() const {
    std::scoped_lock lock(mPool->mMutex);
    return !mPool->mFreeIndices.empty() || mPool->mNext < mPool->mCapacity;
}

