#include "DirectXCommon.h"
#include<format>
#include<cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<vector>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
using namespace Microsoft::WRL;

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"


#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include"DirectXGame/engine/scene/SceneManager.h"

void DirectXCommon::Intialize(WinApp* winApp) {
    
    InitializeFixFPS();
   
	DXGIDevice_->Initialize();
	command_->Initialize(DXGIDevice_.get()); // コマンド
	viewPort_->Initialize(command_.get());   // ビューポート
	scissorRect_->Initialize(command_.get());// シザー
	fence_->Initialize(DXGIDevice_.get(), command_.get()); // フェンス
	dxcCompiler_->Initialize(); // コンパイル
	srvManager_->Initialize(DXGIDevice_.get(), command_.get()); // SRV
	uavManager_->Initialize(DXGIDevice_.get(), command_.get()); // UAV
	rtvManager_->Initialize(DXGIDevice_.get(), command_.get()); // RTV
	dsvManager_->Initialize(DXGIDevice_.get(), command_.get()); // DSV
	swapChain_->Initialize(winApp,DXGIDevice_.get(),command_.get(), rtvManager_.get()); // スワップチェーン
	depthStencil_->Initialize(DXGIDevice_.get(),command_.get(),dsvManager_.get(),srvManager_.get()); // デプスステンシル     
	
	textureManager_->Initialize(command_.get(), DXGIDevice_.get(), srvManager_.get()); // テクスチャマネージャー
	modelManager_->Initialize(this); // モデルマネージャー
	
	renderingCommon_->Initialize(this);
	renderTexture_->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get()); // レンダーテクスチャ
	barrier_->Initialize(command_.get(), swapChain_.get(), renderTexture_.get(),depthStencil_.get()); // バリア

	

	imguiManager_->Initialize(this);

	

}

#pragma region Draw

void DirectXCommon::PreDrawOffscreen() {

	// レンダーターゲット用バリア(描画前)
	barrier_->RenderPre();

	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture_->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture_->GetClearColor().x, renderTexture_->GetClearColor().y,renderTexture_->GetClearColor().z, renderTexture_->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawOffscreen()
{
	// レンダーターゲット用バリア(描画後)
	barrier_->RenderPost();

}

void DirectXCommon::PreDrawSwap() {
	

	// スワップチェーン用バリア(描画前)
	barrier_->SwapPre();
	
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetCurrentBackBufferRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	
	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };  // 任意のクリアカラー（青）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	
	// コマンドを積む
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawSwap() {
	
	// スワップチェーン用バリア(描画後)
	barrier_->SwapPost();

	// コマンドキック
	command_->KickCommand();

	// GPUに画面交換を通知
	swapChain_->Present();

	// フェンス
	fence_->WaitGPU();

	// FPS制限の更新
	UpdateFixFPS();

	// コマンドリセット
	command_->ResetCommand();
}


#pragma endregion // 描画処理



void DirectXCommon::Finalize()
{
	imguiManager_->Finalize();
}

void DirectXCommon::Draw(SceneManager* sceneManager, Entity3DManager* entity3DManager, RenderTexture::PostEffectType type)
{
	///
	// 描画前処理
	GetSrvManager()->PreDraw();

	// レンダーターゲット用の描画準備
	PreDrawOffscreen(); // オフスクリーンのRTV設定

	// 3Dと2D描画
	Draw3D2D(sceneManager, entity3DManager);

	// レンダーターゲット用の描画後処理
	PostDrawOffscreen();

	//



	//


	// スワップチェーン用の描画準備
	PreDrawSwap();


	// レンダーテクスチャ(コピー)
	GetRenderTexture()->Draw(type);

	// ImGuiの描画
	GetImGuiManager()->Draw();

	// スワップチェーン用の描画後処理
	PostDrawSwap();
}

void DirectXCommon::Draw3D2D(SceneManager* sceneManager, Entity3DManager* entity3DManager)
{
	// 3Dオブジェクトの描画
	sceneManager->Draw3D();

	// パーティクル描画
	entity3DManager->GetEffectManager()->GetParticleManager()->Draw();

#ifdef _DEBUG
	if (!sceneManager->IsNowScene("GAMEPLAY")) {
		// デバッグ用のライン描画
		entity3DManager->Get3DLineCommon()->Draw();
	}
#endif // _DEBUG

	// 2Dオブジェクトの描画
	sceneManager->Draw2D();
}

void DirectXCommon::InitializeFixFPS()
{
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回の記録からの経過時間を記録する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒（よりわずかに短い時間）経っていない場合
	if (elapsed < kMinCheckTime) {
		//1 / 60秒経過するまで微小なスリープ繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}
	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

}



