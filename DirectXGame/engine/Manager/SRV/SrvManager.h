#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;
// DirectX
#include <d3d12.h>


#include"DirectXGame/engine/struct/Structs3D.h"

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

class DXGIDevice;
class Command;
// SRV管理
class SrvManager {
public:
	// 初期化
	void Initialize(DXGIDevice* DXGI, Command* Command);

	void Finalize();

	uint32_t Allocate();

	// デスクリプタハンドル計算
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() { return descriptorHeap->GetGPUDescriptorHandleForHeapStart(); }
	ID3D12DescriptorHeap* GetDescriptorHeap() { return descriptorHeap.Get(); }

	// SRV生成関数(テクスチャ用)
	// SRV生成(テクスチャ用)
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource,DirectX::TexMetadata matadata);
	// SRV生成(Structured Buffer用)
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	void PreDraw();

	// SRVセットコマンド
	void SetGraphicsRootdescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	bool IsMaxTextuer();

public:
	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;
private:
	
	// SRV用のデスクリプタサイズ
	uint32_t descriptorSize;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

private:
	DXGIDevice* DXGIDevice_;
	Command* command_;
};
