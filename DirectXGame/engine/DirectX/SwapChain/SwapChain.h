#pragma once

// C++
#include <cmath>
#include <cassert>
#include <memory>
#include <array>
#include <vector>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>
using namespace Microsoft::WRL;

class DXGIDevice;
class Command;
class WinApp;
class RtvManager;

class SwapChain
{
public:
	SwapChain() = default;
	~SwapChain() = default;

	void Initialize(WinApp* winApp, DXGIDevice* dxgi, Command* command, RtvManager* rtvManager);

	// GPUに画面交換を通知
	void Present();
	// 現在のバックバッファのリソースを取得
	ID3D12Resource* GetCurrentBackBufferResource();
	// 描画先のRTVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferRTVHandle();

	// 
	size_t GetBackBufferCount() const { return backBuffers_.size(); }
private:
	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// スワップチェーンリソース作成
	/// </summary>
	void CreateSwapChainResource();

	/// <summary>
	/// RTV作成
	/// </summary>
	void CreateRTV();

private:
	Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain_;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	HRESULT hr_;


	// RTVリソースのインデックス
	uint32_t rtvIndex_[2] = {};
	// バックバッファインデックス
	UINT backBufferIndex_ = 0;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

	DXGI_SWAP_CHAIN_DESC swcDesc_;
private:
	DXGIDevice* DXGIDevice_;
	Command* command_;
	WinApp* winApp_;
	RtvManager* rtvManager_;
};

