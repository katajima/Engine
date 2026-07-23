#pragma once


// C++
#include <memory>

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <wrl.h>
using namespace Microsoft::WRL;


namespace Engine {
	// 前方宣言
	class DXGIDevice;

	/// <summary>
	/// コマンドクラス
	/// </summary>
	class Command
	{
	public:
		Command() = default;
		~Command() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* dxgi);

		/// <summary>
		/// コマンドの実行
		/// </summary>
		void KickCommand();

		/// <summary>
		/// コマンドのリセット
		/// </summary>
		void ResetCommand();

		/// <summary>
		/// キューの取得
		/// </summary>
		ID3D12CommandQueue* GetQueue();
		/// <summary>
		/// アロケータの取得
		/// </summary>
		ID3D12CommandAllocator* GetAllocator();
		/// <summary>
		/// リストの取得
		/// </summary>
		ID3D12GraphicsCommandList* GetList();

	private:

		HRESULT hr_ = S_FALSE;

		// DXGI
		DXGIDevice* dxgi = nullptr;

		// コマンドキュー
		ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
		// コマンドアロケータ
		ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
		// コマンドリスト
		ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	};
}
