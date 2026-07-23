#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


#include"DirectXGame/engine/struct/Structs3D.h"

namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;

	/// <summary>
	/// DSVマネージャー
	/// </summary>
	class DsvManager
	{
	public:
		DsvManager() = default;
		~DsvManager() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* dxgi, Command* command);
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
		/// DSV生成
		/// </summary>
		void CreateDSV(uint32_t dsvIndex, ID3D12Resource* pResource, DXGI_FORMAT format);

		/// <summary>
		/// ディスクリプタヒープを取得
		/// </summary>
		ID3D12DescriptorHeap* GetDescriptorHeap() const;
		/// <summary>
		/// ディスクリプタのサイズを取得
		/// </summary>
		uint32_t GetDescriptorSize() const;
	public:
		// 最大RTV数
		static const uint32_t kMaxDsvCount;

	private:
		// RTV用のデスクリプタサイズ
		uint32_t descriptorSize_;
		// RTV用デスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

		// 次に使用するRTVインデックス
		uint32_t useIndex_ = 0;

	private:

		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
	};
}
