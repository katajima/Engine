#pragma once

#include <d3d12.h>
#include <unordered_map>

class Command;
class SwapChain;
class RenderTexture;
class DepthStencil;
class Barrier
{
public:
	Barrier() = default;
	~Barrier() = default;

	// 
	void Initialize(Command* command, SwapChain* swapChain, RenderTexture* renderTexture, DepthStencil* depthStencil);

public:

	void TransitionResource(ID3D12Resource* res,D3D12_RESOURCE_STATES before,D3D12_RESOURCE_STATES after);

private:

	Command* command_;
	SwapChain* swapChain_;
	RenderTexture* renderTexture_;
	DepthStencil* depthStencil_;
private:

};

