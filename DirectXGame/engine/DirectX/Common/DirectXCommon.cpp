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


	std::unique_ptr<RenderTexture> renderTextere = std::make_unique<RenderTexture>();
	renderTextere->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(),"postEffect0_outline"); // レンダーテクスチャ
	std::unique_ptr<RenderTexture> renderTextere2 = std::make_unique<RenderTexture>();
	renderTextere2->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(),"postEffect1_Extract"); // レンダーテクスチャ
	std::unique_ptr<RenderTexture> renderTextere3 = std::make_unique<RenderTexture>();
	renderTextere3->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(),"postEffect2_Blur"); // レンダーテクスチャ
	std::unique_ptr<RenderTexture> renderTextere4 = std::make_unique<RenderTexture>();
	renderTextere4->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(),"postEffect3_Comb"); // レンダーテクスチャ
	std::unique_ptr<RenderTexture> renderTextere5 = std::make_unique<RenderTexture>();
	renderTextere5->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(),"postEffect4_All"); // レンダーテクスチャ
	renderTextures_.push_back(std::move(renderTextere));
	renderTextures_.push_back(std::move(renderTextere2));
	renderTextures_.push_back(std::move(renderTextere3));
	renderTextures_.push_back(std::move(renderTextere4));
	renderTextures_.push_back(std::move(renderTextere5));
	
	
	
	barrier_->Initialize(command_.get(), swapChain_.get(), renderTextures_[0].get(), depthStencil_.get()); // バリア

	imguiManager_->Initialize(this);
}

#pragma region Draw

void DirectXCommon::PreDrawOffscreen() {

	// レンダーターゲット
	barrier_->TransitionResource(renderTextures_[0]->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTextures_[0]->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTextures_[0]->GetClearColor().x,  renderTextures_[0]->GetClearColor().y, renderTextures_[0]->GetClearColor().z,  renderTextures_[0]->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTextures_[0]->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DirectXCommon::SceneDraw(SceneManager* sceneManager, Entity3DManager* entity3DManager)
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

	sceneManager->Update();

	entity3DManager->GetEffectManager()->GetParticleManager()->Update();

#ifdef _DEBUG
	entity3DManager->Get3DLineCommon()->Update();


	// レンダーテクスチャ
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("RenderTexture")) {
		for (auto& renderTexture : renderTextures_) {
			renderTexture->Update();
			renderTexture->SetCamera(sceneManager->GetCamara());
		}
	}
	ImGui::End();
#endif // _DEBUG
	
	

	
}

void DirectXCommon::Draw(SceneManager* sceneManager, Entity3DManager* entity3DManager)
{
	// シーンを書き出す
	SceneDraw(sceneManager, entity3DManager);

	// レンダーテクスチャ
	DrawRenderTexture(renderTextures_[1].get(), renderTextures_[0].get()); //Outline

	// レンダーテクスチャ
	DrawRenderTexture(renderTextures_[2].get(), renderTextures_[1].get()); // 輝度

	// レンダーテクスチャ
	DrawRenderTexture(renderTextures_[3].get(), renderTextures_[2].get()); // ガウス

	// レンダーテクスチャ
	DrawRenderTexture(renderTextures_[4].get(), renderTextures_[3].get() ,renderTextures_[1].get()); // コンボ

	// スワップチェーン
	PassSwap(renderTextures_[4].get());
}

void DirectXCommon::PreDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	

	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture->GetClearColor().x,  renderTexture->GetClearColor().y, renderTexture->GetClearColor().z,  renderTexture->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	
	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void DirectXCommon::DrawRenderTexture(RenderTexture* renderTextureRenderTreget, RenderTexture* renderTexturePixelSheder, RenderTexture* renderTexturePixelSheder2)
{
	//
	PreDraw(renderTextureRenderTreget);

	// レンダーテクスチャ(コピー)
	renderTexturePixelSheder->Draw(renderTexturePixelSheder2);


	PostDraw(renderTextureRenderTreget);
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



