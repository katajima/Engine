#pragma once

#include <d3d12.h>

class Command;
class SwapChain;
class RenderTexture;

class Barrier
{
public:
	Barrier() = default;
	~Barrier() = default;

	// 
	void Initialize(Command* command, SwapChain* swapChain, RenderTexture* renderTexture);


	void Pre();

	void Post();


private:
	void TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);
private:

	Command* command_;
	SwapChain* swapChain_;
	RenderTexture* renderTexture_;
private:
	D3D12_RESOURCE_BARRIER swapChainBarrier_;

};

