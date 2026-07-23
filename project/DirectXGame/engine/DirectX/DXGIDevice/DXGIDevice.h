#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

#include "DirectXGame/engine/base/Logger.h"
#include "DirectXGame/engine/Utility/StringUtility.h"

namespace Engine {
	/// <summary>
	/// DXGIデバイスクラス
	/// </summary>
	class DXGIDevice
	{
	public:
		DXGIDevice() = default;
		~DXGIDevice() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// D3D12デバイスが保持している生存オブジェクトを出力
		/// </summary>
		void ReportLiveObjects();

		/// <summary>
		/// DXGIデバイスの取得
		/// </summary>
		ID3D12Device* GetDevice() { return device_.Get(); };
		/// <summary>
		/// DXGIファクトリーの取得
		/// </summary>
		IDXGIFactory7* GetFactory() { return dxgiFactory_.Get(); };

		// DescriptorHeapの作成関数
		Microsoft::WRL::ComPtr < ID3D12DescriptorHeap>CreateDescriptorHeap(
			D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

		/// <summary>
		/// Material用のResource作成関数
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

		/// <summary>
		/// Material用のResource作成関数
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> CreateBufferResourceUAV(size_t sizeInBytes);

	private:
		// エラー判別
		HRESULT hr_ = S_FALSE;
		// DXGIファクトリー
		ComPtr <IDXGIFactory7> dxgiFactory_ = nullptr;
		// 使用するアダプタ用の変数
		ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
		// Device
		ComPtr<ID3D12Device> device_ = nullptr;

	};
}
