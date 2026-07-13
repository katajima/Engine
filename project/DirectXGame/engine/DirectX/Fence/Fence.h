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
		// デストラクタ
		~Fence();

		// 初期化
		void Initialize(DXGIDevice* dxgi, Command* command);
		// 終了
		void Finalize();
		// GPUを待機
		void WaitGPU();
	private:
		// フェンス作成
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
