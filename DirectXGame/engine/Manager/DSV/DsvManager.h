#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


#include"DirectXGame/engine/struct/Structs3D.h"

class DXGIDevice;
class Command;

class DsvManager
{
public:
	DsvManager() = default;
	~DsvManager() = default;

	// 初期化
	void Initialize(DXGIDevice* dxgi, Command* command);

	uint32_t Allocate();

	// デスクリプタハンドル計算
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	
	// DSV生成
	void CreateDSV(uint32_t dsvIndex, ID3D12Resource* pResource, DXGI_FORMAT format);

	// ディスクリプタヒープを取得
	ID3D12DescriptorHeap* GetDescriptorHeap() const;
	// ディスクリプタのサイズを取得
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

	DXGIDevice* DXGIDevice_;
	Command* command_;
};

