#pragma once

// C++
#include <cmath>
#include <cassert>
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

#include"DirectXGame/engine/struct/Structs3D.h"

class DXGIDevice;
class Command;
class SrvManager;
class RtvManager;
class RenderingCommon;
class RenderTexture
{
public:
	RenderTexture() = default;
	~RenderTexture() = default;

	void Initialize(DXGIDevice* DXGIDevice,Command* command,SrvManager* srvManager,RtvManager* rvtManager,RenderingCommon* renderingCommon);

	void Draw();

	// クリアカラーを取得
	Vector4 GetClearColor()const;
	// RTVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
	// リソースを取得
	ID3D12Resource* GetResource();


	D3D12_RESOURCE_STATES GetCurrentState() const { return currentState_; }
	void SetCurrentState(D3D12_RESOURCE_STATES state) { currentState_ = state; }

	// レンダーテクスチャのリソースを作成
	void CreateResource();
	// 
	// レンダーテクスチャのリソースを作成
	void CreateResourcePixel();
private:



	// RTVを作成
	void CreateRTV();
	// SRVを作成
	void CreateSRV();

private:
	HRESULT hr_ = S_FALSE;
	// レンダーテクスチャ用リソース
	ComPtr<ID3D12Resource> resource_ = nullptr;
	// RTVインデックス
	uint32_t rtvIndex_ = 0;
	// SRVインデックス
	uint32_t srvIndex_ = 0;
	// クリアカラー(赤)
	const Vector4 clearColor_ = { 1.0f,0.0f,0.0f,1.0f };

	// RenderTexture.h
	D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	
private:
	DXGIDevice* DXGIDevice_;
	Command* command_;
	SrvManager* srvManager_;
	RtvManager* rtvManager_;
	RenderingCommon* renderingCommon_;
};

