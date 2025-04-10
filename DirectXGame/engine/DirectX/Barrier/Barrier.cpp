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

void Barrier::Pre()
{
	// SwapChain用
	// 遷移前
	TransitionResourceState(swapChain_->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//
	/*TransitionResourceState(renderTexture_->GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);*/
}

void Barrier::Post()
{
	TransitionResourceState(swapChain_->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//
	//TransitionResourceState(renderTexture_->GetResource(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Barrier::TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	if (beforeState != afterState)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = beforeState;
		barrier.Transition.StateAfter = afterState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		command_->GetList()->ResourceBarrier(1, &barrier);  // 1はバリアの数
	}
}
