#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>
#include <atomic>

#include"DirectXGame/engine/struct/Structs3D.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"


namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;

	/// <summary>
	/// SRVマネージャークラス
	/// </summary>
	class SrvManager {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* DXGI, Command* Command);
		/// <summary>
		/// アロケート
		/// </summary>
		uint32_t Allocate();

		/// <summary>
		/// デスクリプタハンドル計算
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
		/// <summary>
		/// デスクリプタハンドル計算
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

		/// <summary>
		/// デスクリプタハンドル計算最初
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() { return descriptorHeap->GetGPUDescriptorHandleForHeapStart(); }
		/// <summary>
		/// ディスクプリタヒープ取得
		/// </summary>
		ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap.Get(); }

		// SRV生成関数(テクスチャ用)
		/// <summary>
		/// SRV生成(テクスチャ用)
		/// </summary>
		void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DirectX::TexMetadata metadata);
		/// <summary>
		/// SRV生成(Structured Buffer用)
		/// </summary>
		void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
		//


		// UAV生成関数
		/// <summary>
		/// UAV生成(Structured Buffer用)
		/// </summary>
		void CreateUAVforStructuredBuffer(uint32_t uavIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);
		/// <summary>
		/// UAV生成(テクスチャ用)
		/// </summary>
		void CreateUAVforTexture2D(uint32_t uavIndex, ID3D12Resource* pResource, DXGI_FORMAT format);
		/// <summary>
		/// 描画前処理
		/// </summary>
		void PreDraw();

		/// <summary>
		/// SRVセットコマンド
		/// </summary>
		void SetGraphicsRootdescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);
		/// <summary>
		/// 最大量に達したか
		/// </summary>
		bool IsMaxTexture();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

	public:
		// 最大SRV数(最大テクスチャ枚数)
		static const uint32_t kMaxSRVCount;
	private:

		// SRV用のデスクリプタサイズ
		uint32_t descriptorSize;
		// SRV用デスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

		// 次に使用するSRVインデックス
		std::atomic<uint32_t> useIndex_{ 0 };

	private:
		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
	};
}
