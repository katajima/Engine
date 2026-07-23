#pragma once

// C++
#include <cstdint>
#include <cassert>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;

	/// <summary>
	/// フェンスクラス
	/// </summary>
	class Fence
	{
	public:
		Fence() = default;
		/// <summary>
		/// デストラクタ
		/// </summary>
		~Fence();

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* dxgi, Command* command);
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();
		/// <summary>
		/// GPUを待機
		/// </summary>
		void WaitGPU();
	private:
		/// <summary>
		/// フェンス作成
		/// </summary>
		void CreateFence();

	private:
		DXGIDevice* dxgi = nullptr;
		Command* command = nullptr;

		// フェンス
		ComPtr<ID3D12Fence> fence_ = nullptr;
		// フェンスバリュー
		UINT64 fenceValue_ = 0;
		// フェンスイベント
		HANDLE fenceEvent_ = nullptr;
		// エラー半別
		HRESULT hr_ = S_FALSE;


	};
}
