#include "Barrier.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"

void Barrier::Initialize(Command* command)
{
    command_ = command;
}

void Barrier::TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    if (before != after)
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = res;
        barrier.Transition.StateBefore = before;
        barrier.Transition.StateAfter = after;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        command_->GetList()->ResourceBarrier(1, &barrier);
    }
}

void Barrier::TransitionResource(ID3D12Resource* res, D3D12_RESOURCE_STATES newState)
{
    auto it = resourceStates_.find(res);
    D3D12_RESOURCE_STATES currentState = D3D12_RESOURCE_STATE_COMMON; // 初期状態は仮に COMMON に

    if (it != resourceStates_.end()) {
        currentState = it->second;
    }

    if (currentState != newState) {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Transition.pResource = res;
        barrier.Transition.StateBefore = currentState;
        barrier.Transition.StateAfter = newState;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        command_->GetList()->ResourceBarrier(1, &barrier);

        resourceStates_[res] = newState; // 状態更新
    }
}

void Barrier::RegisterInitialState(ID3D12Resource* res, D3D12_RESOURCE_STATES state)
{
    resourceStates_[res] = state;
}




