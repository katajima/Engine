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
	rtvManager_->Initialize(DXGIDevice_.get(), command_.get()); // RTV
	dsvManager_->Initialize(DXGIDevice_.get(), command_.get()); // DSV
	swapChain_->Initialize(winApp,DXGIDevice_.get(),command_.get(), rtvManager_.get()); // スワップチェーン
	depthStencil_->Initialize(DXGIDevice_.get(),command_.get(),dsvManager_.get(),srvManager_.get()); // デプスステンシル     
	
	textureManager_->Initialize(command_.get(), DXGIDevice_.get(), srvManager_.get()); // テクスチャマネージャー
	modelManager_->Initialize(this); // モデルマネージャー
	
	renderingCommon_->Initialize(this);
	barrier_->Initialize(command_.get()); // バリア

	// ポストエフェクトマネージャー(レンダリング関係のマネージャー)
	postEffectManager_->Intialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(), depthStencil_.get(), barrier_.get(), scissorRect_.get(), viewPort_.get());

	postEffectManager_->AddRenderTexture("postEffect0_outline");
	postEffectManager_->AddRenderTexture("postEffect1_Extract");
	postEffectManager_->AddRenderTexture("postEffect2_Blur");
	postEffectManager_->AddRenderTexture("postEffect3_Comb");
	postEffectManager_->AddRenderTexture("postEffect4_All");

	imguiManager_->Initialize(this);
}

#pragma region Draw

void DirectXCommon::SceneDraw(SceneManager* sceneManager, Entity3DManager* entity3DManager)
{
	///
	// 描画前処理
	GetSrvManager()->PreDraw();

	// レンダーターゲット用の描画準備
	postEffectManager_->PreDrawOffscreen(); // オフスクリーンのRTV設定

	// 3Dと2D描画
	Draw3D2D(sceneManager, entity3DManager);

	// レンダーターゲット用の描画後処理
	postEffectManager_->PostDrawOffscreen();

}

void DirectXCommon::PreDrawSwap() {
	
	// スワップチェーン用
	barrier_->TransitionResource(swapChain_->GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

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
	
	// スワップチェーン用
	barrier_->TransitionResource(swapChain_->GetCurrentBackBufferResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
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

void DirectXCommon::PassSwap(RenderTexture* renderTexture)
{
	// スワップチェーン用の描画準備
	PreDrawSwap();

	// レンダーテクスチャ(コピー)
	renderTexture->Draw();

	
	// ImGuiの描画
	GetImGuiManager()->Draw();

	// スワップチェーン用の描画後処理
	PostDrawSwap();
}


#pragma endregion // 描画処理



void DirectXCommon::Finalize()
{
	imguiManager_->Finalize();
}

void DirectXCommon::Update(SceneManager* sceneManager, Entity3DManager* entity3DManager)
{
	// ライト
	entity3DManager->GetLightManager()->Update();

#ifdef _DEBUG
	entity3DManager->UpdateImgui();
#endif // _DEBUG

	sceneManager->Update();

	entity3DManager->GetEffectManager()->GetParticleManager()->Update();


	postEffectManager_->Update(sceneManager->GetCamara());

#ifdef _DEBUG
	
	entity3DManager->Get3DLineCommon()->Update();

#endif // _DEBUG

	

	
}

void DirectXCommon::Draw(SceneManager* sceneManager, Entity3DManager* entity3DManager)
{
	// シーンを書き出す
	SceneDraw(sceneManager, entity3DManager);

	postEffectManager_->AllPostEffect();

	// スワップチェーン
	PassSwap(postEffectManager_->GetEndRenderTexture());

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
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	while (true) {
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

		if (elapsed >= kMinTime) {
			break;
		}

		// 残り時間が2ms以上あるなら軽くSleepする
		if (kMinTime - elapsed > std::chrono::microseconds(2000)) {
			std::this_thread::sleep_for(std::chrono::microseconds(1000)); // 1ms Sleep
		}
		// それ以外はbusy loopで待機
	}

	reference_ = std::chrono::steady_clock::now();

}



