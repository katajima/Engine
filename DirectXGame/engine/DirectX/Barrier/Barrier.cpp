#include "Barrier.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"

void Barrier::Initialize(Command* command, SwapChain* swapChain, RenderTexture* renderTexture, DepthStencil* depthStencil)
{
    command_ = command;
    swapChain_ = swapChain;
    renderTexture_ = renderTexture;
    depthStencil_ = depthStencil;
}

void Barrier::RenderPre()
{
    // レンダーターゲット
    TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // デプスステンシル
    TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

}


void Barrier::RenderPost()
{
    // レンダーターゲット
    TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    // デプスステンシル
    TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

void Barrier::SwapPre()
{
    // スワップチェーン用
    TransitionResource(swapChain_->GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

}

void Barrier::SwapPost()
{
    // スワップチェーン用
    TransitionResource(swapChain_->GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
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




