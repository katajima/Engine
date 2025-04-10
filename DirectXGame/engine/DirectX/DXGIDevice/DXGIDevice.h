#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

#include "DirectXGame/engine/base/Logger.h"
#include "DirectXGame/engine/base/StringUtility.h"

class DXGIDevice
{
public:
	DXGIDevice() = default;
	~DXGIDevice() = default;

	void Initialize();


	// DXGIデバイスの取得
	ID3D12Device* GetDevice() { return device_.Get(); };
	// DXGIファクトリーの取得
	IDXGIFactory7* GetFactory() { return dxgiFactory_.Get(); };
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

