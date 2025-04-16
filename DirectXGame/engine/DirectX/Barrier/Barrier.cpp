#include "Barrier.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"


void Barrier::Initialize(Command* command, SwapChain* swapChain, RenderTexture* renderTexture)
{
    command_ = command;
    swapChain_ = swapChain;
    renderTexture_ = renderTexture;
}

void Barrier::RenderPre()
{
    D3D12_RESOURCE_BARRIER renderBarrier = {};
    renderBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    renderBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    renderBarrier.Transition.pResource = renderTexture_->GetResource();

    // リソースの状態を遷移させる
    renderBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    renderBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    renderBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // バリアを適用
    command_->GetList()->ResourceBarrier(1, &renderBarrier);

    // デバッグログで状態の確認（例:状態の遷移後に再確認する）
    OutputDebugStringA(">>> RenderPre: Resource state transition complete.\n");
}


void Barrier::RenderPost()
{
    D3D12_RESOURCE_BARRIER renderBarrier = {};
    renderBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    renderBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    renderBarrier.Transition.pResource = renderTexture_->GetResource();
    renderBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    renderBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE; // 戻す
    renderBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    command_->GetList()->ResourceBarrier(1, &renderBarrier);
}

void Barrier::SwapPre()
{
    // swapChainbarrierの宣言と初期化
    D3D12_RESOURCE_BARRIER swapChainbarrier = {};
    swapChainbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    swapChainbarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

    // バリアを張る対象のリソース。現在のバックバッファに対して行う
    swapChainbarrier.Transition.pResource = swapChain_->GetCurrentBackBufferResource();

    // 遷移前(現在)のResourceState
    swapChainbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

    // 遷移後のResourceState
    swapChainbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // TransitionBarrierを張る
    command_->GetList()->ResourceBarrier(1, &swapChainbarrier);

    OutputDebugStringA("RenderPre() called\n");
}

void Barrier::SwapPost()
{
    // swapChainbarrierの宣言と初期化
    D3D12_RESOURCE_BARRIER swapChainbarrier = {};
    swapChainbarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    swapChainbarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

    // バリアを張る対象のリソース。現在のバックバッファに対して行う
    swapChainbarrier.Transition.pResource = swapChain_->GetCurrentBackBufferResource();

    // 遷移前(現在)のResourceState
    swapChainbarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;

    // 遷移後のResourceState
    swapChainbarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

    // TransitionBarrierを張る
    command_->GetList()->ResourceBarrier(1, &swapChainbarrier);

}




