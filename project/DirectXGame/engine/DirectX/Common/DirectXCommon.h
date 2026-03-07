#pragma once
#include <windows.h>
#include<cstdint>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<wrl.h>
#include <array>
#include"DirectXGame/engine/base/Logger.h"
#include"DirectXGame/engine/Utility/StringUtility.h"
#include"DirectXGame/engine/base/WinApp/WinApp.h"
#include<chrono>
#include <thread>
#include<vector>


#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

#include"DirectXGame/engine/struct/Structs3D.h"

#include "DirectXGame/engine/DirectX/D3DResourceLeakchecker/D3DResourceLeakchecker.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"
#include "DirectXGame/engine/DirectX/Fence/Fence.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"
#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/base/Imgui/ImGuiManager.h"


#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Manager/DSV/DsvManager.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"

#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/Offscreen/PostEffectManager.h"


namespace Engine {
	// 前方宣言
	class RenderTexture;
	class RenderingCommon;
	class EntityManager;
	class SceneManager;

	/// <summary>
	/// DirectXの共通クラス
	/// </summary>
	/// 
	class DirectXCommon
	{
	public: // メンバ関数

		// 初期化
		void Intialize(WinApp* winApp);

		//終了処理
		void Finalize();
		// 更新
		void Update(SceneManager* sceneManager, EntityManager* entity3DManager);
		// 描画
		void Draw(SceneManager* sceneManager, EntityManager* entity3DManager);

	private:
		// シーンの画面を書き出す
		void SceneDraw(SceneManager* sceneManager, EntityManager* entity3DManager);
	private:
		// スワップチェーンにレンダーターゲットを渡す
		void PassSwap(SceneManager* sceneManager, RenderTexture* renderTexture);
	private:
		// 3D2D描画
		void Draw3D2D(SceneManager* sceneManager, EntityManager* entity3DManager);

		//FPS固定初期化
		void InitializeFixFPS();
		//FPS固定更新
		void UpdateFixFPS();

	public:

		// バックバッファの数を取得
		size_t GetBackBufferCount() const { return swapChain_->GetBackBufferCount(); }

		// デバイス取得
		Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return DXGIDevice_->GetDevice(); }
		// コマンドリスト取得
		Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> GetCommandList() { return command_->GetList(); }
		// SRVマネージャー取得
		SrvManager* GetSrvManager() { return  srvManager_.get(); }
		// RTVマネージャー取得
		RtvManager* GetRtvManager() { return  rtvManager_.get(); }
		// テクスチャマネージャー取得
		TextureManager* GetTextureManager() { return textureManager_.get(); }
		// モデルマネージャー取得
		ModelManager* GetModelManager() { return modelManager_.get(); }
		// DXGIデバイス取得
		DXGIDevice* GetDXGIDevice() { return DXGIDevice_.get(); }
		// コマンド取得
		Command* GetCommand() { return command_.get(); }
		// DXコンパイラ取得
		DXCCompiler* GetDXCCompiler() { return dxcCompiler_.get(); }
		// ImGuiマネージャー取得
		ImGuiManager* GetImGuiManager() { return imguiManager_.get(); }
		// レンダリング共通クラス取得
		RenderingCommon* GetRenderingCommon() { return renderingCommon_.get(); }
		// デプスステンシル取得
		DepthStencil* GetDepthStencil() { return depthStencil_.get(); }
		// バリア取得
		Barrier* GetBarrier() { return barrier_.get(); }
		// ポストエフェクトマネージャー取得
		PostEffectManager* GetPostEffectManager() { return postEffectManager_.get(); }
		// スワップチェーン取得
		SwapChain* GetSwapChain() { return swapChain_.get(); }
		// フェンス取得
		Fence* GetFence() { return fence_.get(); }
		// シザーレクト取得
		ScissorRect* GetScissorRect() { return scissorRect_.get(); };
		// ビューポート取得
		ViewPort* GetViewPort() { return viewPort_.get(); }
	private:
		std::unique_ptr<DXGIDevice> DXGIDevice_ = std::make_unique<DXGIDevice>();			     // デバイス
		std::unique_ptr<Command> command_ = std::make_unique<Command>();					     // コマンド
		std::unique_ptr<ScissorRect> scissorRect_ = std::make_unique<ScissorRect>();		     // シザー
		std::unique_ptr<ViewPort> viewPort_ = std::make_unique<ViewPort>();					     // ビューポート
		std::unique_ptr<Fence> fence_ = std::make_unique<Fence>();							     // フェンス
		std::unique_ptr<DXCCompiler> dxcCompiler_ = std::make_unique<DXCCompiler>();		     // コンパイル
		std::unique_ptr<SwapChain> swapChain_ = std::make_unique<SwapChain>();				     // スワップチェーン 
		std::unique_ptr<RtvManager> rtvManager_ = std::make_unique<RtvManager>();			     // RTVマネージャー 
		std::unique_ptr<SrvManager> srvManager_ = std::make_unique<SrvManager>();			     // SRVマネージャー 
		std::unique_ptr<DsvManager> dsvManager_ = std::make_unique<DsvManager>();			     // DRVマネージャー 
		std::unique_ptr<DepthStencil> depthStencil_ = std::make_unique<DepthStencil>();		     // デプスステンシル 
		std::unique_ptr<Barrier> barrier_ = std::make_unique<Barrier>();					     // バリア 
		std::unique_ptr<TextureManager> textureManager_ = std::make_unique<TextureManager>();    // テクスチャマネージャー 
		std::unique_ptr<ModelManager> modelManager_ = std::make_unique<ModelManager>();		     // モデルマネージャー
		std::unique_ptr<RenderingCommon> renderingCommon_ = std::make_unique<RenderingCommon>(); // レンダリング

		std::unique_ptr<PostEffectManager> postEffectManager_ = std::make_unique<PostEffectManager>(); // ポストエフェクト

		// ImGuiマネージャー
		std::unique_ptr <ImGuiManager> imguiManager_ = std::make_unique<ImGuiManager>();

		// 記録時間(FPS固定用)
		std::chrono::steady_clock::time_point reference_;
	};
}