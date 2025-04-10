#pragma once
#include <windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<wrl.h>
#include <array>
#include"Logger.h"
#include"StringUtility.h"
#include"WinApp.h"
#include<chrono>
#include <thread>
#include<vector>


#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

#include"DirectXGame/engine/struct/Structs3D.h"


#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"
#include "DirectXGame/engine/DirectX/Fence/Fence.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"
#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"






class RenderingCommon;

class DirectXCommon
{
public: // メンバ関数
	
	// CPUHandle
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	// GPUHandle
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	// 初期化
	void Intialize(WinApp* winApp);

	// 描画前処理
	void PreDrawOffscreen();
	// 描画後処理
	void PostDrawOffscreen();


	// 描画前処理
	void PreDrawSwap();
	// 描画後処理
	void PostDrawSwap();

	//終了処理
	void Finalize();

private:
	D3D12_RESOURCE_STATES swapChainState_; // スワップチェインの状態
	D3D12_RESOURCE_STATES renderTextureState_; // rendertextureの状態

	D3D12_RESOURCE_BARRIER barrier1;

	void TransitionResourceState(ID3D12Resource* resource,
		D3D12_RESOURCE_STATES beforeState,
		D3D12_RESOURCE_STATES afterState);

	
	
	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateRenderTargets();

	///
	///
	/// 
	void CreateDescriptorHeap();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// DepthStencilView(DSV)初期化
	/// </summary>
	void InitializeDepthStencilView();


	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();

public:

	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return swapChain_->GetBackBufferCount(); }

	// DescriptorHeapの作成関数
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap>CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// Material用のResource作成関数
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr <ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);


	//データを転送するUploadTextureData関数を作る
	[[nodiscard]]
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return DXGIDevice_->GetDevice(); }

	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> GetCommandList() { return command_->GetList(); }

	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> GetDsvDescriptorHeap() { return dsvDescriptorHeap; }


	void CreateRenderTexture();

	//Microsoft::WRL::ComPtr < ID3D12Resource> GetRenderTextureResource() const { return renderTextureResource_; }

	//Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource_;

	uint32_t index;
	
private:
	std::unique_ptr<DXGIDevice> DXGIDevice_ = std::make_unique<DXGIDevice>();   // デバイス
	std::unique_ptr<Command> command_ = std::make_unique<Command>();            // コマンド
	std::unique_ptr<ScissorRect> scissorRect_ = std::make_unique<ScissorRect>();// シザー
	std::unique_ptr<ViewPort> viewPort_ = std::make_unique<ViewPort>();         // ビューポート
	std::unique_ptr<Fence> fence_ = std::make_unique<Fence>();                  // フェンス
	std::unique_ptr<DXCCompiler> dxcCompiler_ = std::make_unique<DXCCompiler>();// コンパイル
	std::unique_ptr<SwapChain> swapChain_ = std::make_unique<SwapChain>();      // スワップチェーン 
	std::unique_ptr<RtvManager> rtvManager_ = std::make_unique<RtvManager>();   // RTVマネージャー 


	
	/*Microsoft::WRL::ComPtr < IDXGISwapChain4> swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;*/
	Microsoft::WRL::ComPtr < ID3D12Resource> depthStencilResource;
	//std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvTexHandle;

	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	
	//D3D12_RENDER_TARGET_VIEW_DESC rtvTexDesc_;
	
	
	Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> dsvDescriptorHeap;
	

	D3D12_RESOURCE_BARRIER barrier;
	uint32_t desriptorSizeRTV;
	uint32_t desriptorSizeDSV;

	//std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

	
public:
	// CPUHandle
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	// GPUHandle
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);


public:
	
	
	////------CompileShader------////
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePach,
		//CompileShaderに使用するProfile
		const wchar_t* profile);

};

