#pragma once

#include <d3d12.h>
#include <unordered_map>

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

	// レンダーターゲット用バリア(Pre)
	void RenderPre();

	// レンダーターゲット用バリア(Post)
	void RenderPost();

	// スワップチェーン用バリア(Pre)
	void SwapPre();

	// スワップチェーン用バリア(Post)
	void SwapPost();

private:

private:

	Command* command_;
	SwapChain* swapChain_;
	RenderTexture* renderTexture_;

private:
	D3D12_RESOURCE_STATES renderTextureState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//D3D12_RESOURCE_BARRIER  swapChainbarrier{};
	// Resource状態の追跡マップをメンバ変数に追加（ヘッダなど）
	//std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> resourceStates;
};

