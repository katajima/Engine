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
#include "DirectXGame/engine/DirectX/ShadowMap/ShadowMap.h"
#include "DirectXGame/engine/base/Imgui/ImGuiManager.h"


#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Manager/DSV/DsvManager.h"
#include "DirectXGame/engine/Manager/RTV/RtvManager.h"

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
/// <summary>
/// DirectXCommonを管理・実装するクラス。
/// </summary>
	class DirectXCommon
	{
	public: // メンバ関数

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(WinApp* winApp);

		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update(SceneManager* sceneManager, EntityManager* entity3DManager);
		/// <summary>
		/// 描画
		/// </summary>
		void Draw(SceneManager* sceneManager, EntityManager* entity3DManager);
		/// <summary>
		/// シャドウマップのデバッグ表示
		/// </summary>
		void RenderShadowMapDebugImGui();

	private:
		/// <summary>
		/// シーンの画面を書き出す
		/// </summary>
		void SceneDraw(SceneManager* sceneManager, EntityManager* entity3DManager);
	private:
		/// <summary>
		/// スワップチェーンにレンダーターゲットを渡す
		/// </summary>
		void PassSwap(SceneManager* sceneManager, RenderTexture* renderTexture);
	private:
		/// <summary>
		/// 3D2D描画
		/// </summary>
		void Draw3D2D(SceneManager* sceneManager, EntityManager* entity3DManager);

		/// <summary>
		/// FPS固定初期化
		/// </summary>
		void InitializeFixFPS();
		/// <summary>
		/// FPS固定更新
		/// </summary>
		void UpdateFixFPS();

	public:

		/// <summary>
		/// バックバッファの数を取得
		/// </summary>
		size_t GetBackBufferCount() const { return swapChain_->GetBackBufferCount(); }

		/// <summary>
		/// デバイス取得
		/// </summary>
		Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() { return DXGIDevice_->GetDevice(); }
		/// <summary>
		/// コマンドリスト取得
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12GraphicsCommandList> GetCommandList() { return command_->GetList(); }
		/// <summary>
		/// SRVマネージャー取得
		/// </summary>
		SrvManager* GetSrvManager() { return  srvManager_.get(); }
		/// <summary>
		/// RTVマネージャー取得
		/// </summary>
		RtvManager* GetRtvManager() { return  rtvManager_.get(); }
		/// <summary>
		/// テクスチャマネージャー取得
		/// </summary>
		TextureManager* GetTextureManager() { return textureManager_.get(); }
		/// <summary>
		/// モデルマネージャー取得
		/// </summary>
		ModelManager* GetModelManager() { return modelManager_.get(); }
		/// <summary>
		/// DXGIデバイス取得
		/// </summary>
		DXGIDevice* GetDXGIDevice() { return DXGIDevice_.get(); }
		/// <summary>
		/// コマンド取得
		/// </summary>
		Command* GetCommand() { return command_.get(); }
		/// <summary>
		/// DXコンパイラ取得
		/// </summary>
		DXCCompiler* GetDXCCompiler() { return dxcCompiler_.get(); }
		/// <summary>
		/// ImGuiマネージャー取得
		/// </summary>
		ImGuiManager* GetImGuiManager() { return imguiManager_.get(); }
		/// <summary>
		/// レンダリング共通クラス取得
		/// </summary>
		RenderingCommon* GetRenderingCommon() { return renderingCommon_.get(); }
		/// <summary>
		/// デプスステンシル取得
		/// </summary>
		DepthStencil* GetDepthStencil() { return depthStencil_.get(); }
		/// <summary>
		/// シャドウマップ取得
		/// </summary>
		ShadowMap* GetShadowMap() { return shadowMap_.get(); }
		/// <summary>
		/// バリア取得
		/// </summary>
		Barrier* GetBarrier() { return barrier_.get(); }
		/// <summary>
		/// ポストエフェクトマネージャー取得
		/// </summary>
		PostEffectManager* GetPostEffectManager() { return postEffectManager_.get(); }
		/// <summary>
		/// スワップチェーン取得
		/// </summary>
		SwapChain* GetSwapChain() { return swapChain_.get(); }
		/// <summary>
		/// フェンス取得
		/// </summary>
		Fence* GetFence() { return fence_.get(); }
		/// <summary>
		/// シザーレクト取得
		/// </summary>
		ScissorRect* GetScissorRect() { return scissorRect_.get(); };
		/// <summary>
		/// ビューポート取得
		/// </summary>
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
		std::unique_ptr<ShadowMap> shadowMap_ = std::make_unique<ShadowMap>();				     // シャドウマップ
		std::unique_ptr<TextureManager> textureManager_ = std::make_unique<TextureManager>();    // テクスチャマネージャー 
		std::unique_ptr<ModelManager> modelManager_ = std::make_unique<ModelManager>();		     // モデルマネージャー
		std::unique_ptr<RenderingCommon> renderingCommon_ = std::make_unique<RenderingCommon>(); // レンダリング

		std::unique_ptr<PostEffectManager> postEffectManager_ = std::make_unique<PostEffectManager>(); // ポストエフェクト

		/// <summary>
		/// ImGuiマネージャー
		/// </summary>
		std::unique_ptr <ImGuiManager> imguiManager_ = std::make_unique<ImGuiManager>();

		// 記録時間(FPS固定用)
		std::chrono::steady_clock::time_point reference_;
	};
}
