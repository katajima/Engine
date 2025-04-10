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
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"

#include "DirectXGame/engine/Manager/UAV/UavManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Manager/DSV/DsvManager.h"






class RenderingCommon;

class DirectXCommon
{
public: // メンバ関数
	
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


	//FPS固定初期化
	void InitializeFixFPS();
	//FPS固定更新
	void UpdateFixFPS();

public:

	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return swapChain_->GetBackBufferCount(); }


	Microsoft::WRL::ComPtr <ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	//データを転送するUploadTextureData関数を作る
	[[nodiscard]]
	Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return DXGIDevice_->GetDevice(); }
	DXGIDevice* GetDXGIDevice() { return DXGIDevice_.get(); }

	Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> GetCommandList() { return command_->GetList(); }

	SrvManager* GetSrvManager() { return  srvManager_.get(); }

	RenderTexture* GetRenderTexture() { return renderTexture_.get(); }

	
private:
	std::unique_ptr<DXGIDevice> DXGIDevice_ = std::make_unique<DXGIDevice>();			// デバイス
	std::unique_ptr<Command> command_ = std::make_unique<Command>();					// コマンド
	std::unique_ptr<ScissorRect> scissorRect_ = std::make_unique<ScissorRect>();		// シザー
	std::unique_ptr<ViewPort> viewPort_ = std::make_unique<ViewPort>();					// ビューポート
	std::unique_ptr<Fence> fence_ = std::make_unique<Fence>();							// フェンス
	std::unique_ptr<DXCCompiler> dxcCompiler_ = std::make_unique<DXCCompiler>();		// コンパイル
	std::unique_ptr<SwapChain> swapChain_ = std::make_unique<SwapChain>();				// スワップチェーン 
	std::unique_ptr<RtvManager> rtvManager_ = std::make_unique<RtvManager>();			// RTVマネージャー 
	std::unique_ptr<UavManager> uavManager_ = std::make_unique<UavManager>();			// UAVマネージャー 
	std::unique_ptr<SrvManager> srvManager_ = std::make_unique<SrvManager>();			// SRVマネージャー 
	std::unique_ptr<DsvManager> dsvManager_ = std::make_unique<DsvManager>();			// DRVマネージャー 
	std::unique_ptr<DepthStencil> depthStencil_ = std::make_unique<DepthStencil>();		// デプスステンシル 
	std::unique_ptr<Barrier> barrier_ = std::make_unique<Barrier>();					// バリア 
	std::unique_ptr<RenderTexture> renderTexture_ = std::make_unique<RenderTexture>();	// レンダーテクスチャ 


	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;
public:

public:
	////------CompileShader------////
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//CompileShaderするShaderファイルへのパス
		const std::wstring& filePach,
		//CompileShaderに使用するProfile
		const wchar_t* profile);

};

