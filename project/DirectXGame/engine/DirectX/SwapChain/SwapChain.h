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

namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;
	class WinApp;
	class RtvManager;
	class Barrier;
	class ScissorRect;
	class ViewPort;
	class Fence;

	// スワップチェーン
/// <summary>
/// SwapChainを管理・実装するクラス。
/// </summary>
	class SwapChain
	{
	public:
		SwapChain() = default;
		~SwapChain() = default;


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(WinApp* winApp, DXGIDevice* dxgi, Command* command, RtvManager* rtvManager, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort, Fence* fence);

		/// <summary>
		/// GPUに画面交換を通知
		/// </summary>
		void Present();
		/// <summary>
		/// 現在のバックバッファのリソースを取得
		/// </summary>
		ID3D12Resource* GetCurrentBackBufferResource();
		/// <summary>
		/// 描画先のRTVハンドルを取得
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferRTVHandle();

		/// <summary>
		/// バッファカウント取得
		/// </summary>
		size_t GetBackBufferCount() const { return backBuffers_.size(); }

		/// <summary>
		/// 描画前
		/// </summary>
		void PreDraw();
		/// <summary>
		/// 描画後
		/// </summary>
		void PostDraw();
		/// <summary>
		/// リサイズ
		/// </summary>
		void Resize(int width, int height);

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
		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
		WinApp* winApp = nullptr;
		RtvManager* rtvManager = nullptr;
		Barrier* barrier = nullptr;
		ScissorRect* scissorRect = nullptr;
		ViewPort* viewPort = nullptr;
		Fence* fence = nullptr;
	};
}
