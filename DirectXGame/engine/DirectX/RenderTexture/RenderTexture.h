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

#include "DirectXGame/engine/Offscreen/PostEffectData.h"

class DXGIDevice;
class Command;
class SrvManager;
class RtvManager;
class RenderingCommon;
class Camera;
class RenderTexture
{
public:
	RenderTexture() = default;
	~RenderTexture() = default;

	void Initialize(DXGIDevice* DXGIDevice,Command* command,SrvManager* srvManager,RtvManager* rvtManager,RenderingCommon* renderingCommonm,const std::string name);

	void Update();

	void Draw();

	void SetCamera(Camera* camera) {camera_ = camera;}

	void SetOtherSrvIndex(uint32_t index) { otherSrvIndex_ = index; };



	uint32_t GetSrvIndex() { return srvIndex_; }
	// クリアカラーを取得
	Vector4 GetClearColor()const;
	// RTVハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
	// リソースを取得
	ID3D12Resource* GetResource();

	// SRVGPUハンドル取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();
	// SRVCPUハンドル取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle();

private:
	// レンダーテクスチャのリソースを作成
	void CreateResource();
	// 
	// レンダーテクスチャのリソースを作成
	void CreateResourcePixel();

	// RTVを作成
	void CreateRTV();
	// SRVを作成
	void CreateSRV();


	void UpdateImgui();

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

	std::string name_;

	// もらってくるSRVインデックス
	uint32_t otherSrvIndex_ = 0;

private:
	DXGIDevice* DXGIDevice_;
	Command* command_;
	SrvManager* srvManager_;
	RtvManager* rtvManager_;
	RenderingCommon* renderingCommon_;
	Camera* camera_;
public:
	PostEffectType type_;


};

