#pragma once

// C++
#include <cstdint>
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX
#include <d3d12.h>

class DXGIDevice;
class Command;
class DsvManager;

class DepthStencil
{
public:
	DepthStencil() = default;
	~DepthStencil() = default;

	// 初期化
	void Initialize(DXGIDevice* dxgi, Command* command, DsvManager* dsvManager);
	// デプスをクリア
	void ClearDepthView();
	// デプスステンシルリソースのCPUハンドルの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleDepthStencilResorce();
private:
	// デプスステンシルリソースの作成
	void CreateDepthStencilView();
private:
	// デプスステンシルリソース
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	// デプスステンシルリソースのインデックス
	uint32_t dsvIndex_ = 0;



	DXGIDevice* DXGIDevice_;
	Command* command_;
	DsvManager* dsvManager_;

};


