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
class SrvManager;
class DepthStencil
{
public:
	DepthStencil() = default;
	~DepthStencil() = default;

	// 初期化
	void Initialize(DXGIDevice* dxgi, Command* command, DsvManager* dsvManager, SrvManager* srvManager);
	// デプスをクリア
	void ClearDepthView();
	// デプスステンシルリソースのCPUハンドルの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleDepthStencilResorce();

	// デプスSRVハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleDepthSRV();

	// デプスSRVインデックス取得
	uint32_t GetDepthSrvIndex();

	// リソースを取得
	ID3D12Resource* GetResource();


private:
	// デプスステンシルリソースの作成
	void CreateDepthStencilView();
private:
	// デプスステンシル用リソース
	ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	// デプスステンシル用リソースのDSVインデックス
	uint32_t dsvIndex_ = 0;
	// デプスステンシル用リソースのSRVインデックス
	uint32_t srvIndex_ = 0;



	DXGIDevice* DXGIDevice_;
	Command* command_;
	DsvManager* dsvManager_;
	SrvManager* srvManager_;

};


