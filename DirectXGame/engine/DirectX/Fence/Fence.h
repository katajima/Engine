#pragma once

// C++
#include <cstdint>
#include <cassert>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


class DXGIDevice;
class Command;
class Fence
{
public:
	Fence() = default;
	~Fence() = default;

	// 初期化
	void Initialize(DXGIDevice* dxgi, Command* command);
	// GPUを待機
	void WaitGPU();
private:
	// フェンス作成
	void CreateFence();

private:
	DXGIDevice* dxgi_ = nullptr;
	Command* command_ = nullptr;

	// フェンス
	ComPtr<ID3D12Fence> fence_ = nullptr;
	// フェンスバリュー
	UINT64 fenceValue_ = 0;
	// フェンスイベント
	HANDLE fenceEvent_;
	// エラー半別
	HRESULT hr_ = S_FALSE;
	

};

