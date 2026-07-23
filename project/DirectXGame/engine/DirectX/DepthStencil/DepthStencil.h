#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX
#include <d3d12.h>

namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;
	class DsvManager;
	class SrvManager;

	/// <summary>
	/// デプスステンシルクラス
	/// </summary>
	class DepthStencil
	{
	public:
		DepthStencil() = default;
		~DepthStencil() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* dxgi, Command* command, DsvManager* dsvManager, SrvManager* srvManager);
		/// <summary>
		/// デプスをクリア
		/// </summary>
		void ClearDepthView();
		/// <summary>
		/// デプスステンシルリソースのCPUハンドルの取得
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleDepthStencilResource();

		/// <summary>
		/// デプスSRVハンドル取得
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleDepthSRV();

		/// <summary>
		/// デプスSRVインデックス取得
		/// </summary>
		uint32_t GetDepthSrvIndex() const;

		/// <summary>
		/// リソースを取得
		/// </summary>
		ID3D12Resource* GetResource();
		/// <summary>
		/// SRVGPUハンドル取得
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();

	private:
		/// <summary>
		/// デプスステンシルリソースの作成
		/// </summary>
		void CreateDepthStencilView();
	private:
		// デプスステンシル用リソース
		ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
		// デプスステンシル用リソースのDSVインデックス
		uint32_t dsvIndex_ = 0;
		// デプスステンシル用リソースのSRVインデックス
		uint32_t srvIndex_ = 0;



		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
		DsvManager* dsvManager = nullptr;
		SrvManager* srvManager = nullptr;

	};
}

