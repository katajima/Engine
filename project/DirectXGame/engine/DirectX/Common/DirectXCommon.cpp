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

#include "DirectXGame/engine/Offscreen/RenderingCommon.h"
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include"DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/scene/SceneManager.h"

void Engine::DirectXCommon::Intialize(WinApp* winApp) {

	InitializeFixFPS();	// 固定FPS初期化

	DXGIDevice_->Initialize();
	command_->Initialize(DXGIDevice_.get()); // コマンド
	viewPort_->Initialize(command_.get());   // ビューポート
	scissorRect_->Initialize(command_.get());// シザー
	fence_->Initialize(DXGIDevice_.get(), command_.get()); // フェンス
	dxcCompiler_->Initialize(); // コンパイル
	srvManager_->Initialize(DXGIDevice_.get(), command_.get()); // SRV
	rtvManager_->Initialize(DXGIDevice_.get(), command_.get()); // RTV
	dsvManager_->Initialize(DXGIDevice_.get(), command_.get()); // DSV
	depthStencil_->Initialize(DXGIDevice_.get(), command_.get(), dsvManager_.get(), srvManager_.get()); // デプスステンシル     


	textureManager_->Initialize(command_.get(), DXGIDevice_.get(), srvManager_.get()); // テクスチャマネージャー
	modelManager_->Initialize(this); // モデルマネージャー

	renderingCommon_->Initialize(this);
	barrier_->Initialize(command_.get()); // バリア
	shadowMap_->Initialize(this); // シャドウマップ


	swapChain_->Initialize(winApp, DXGIDevice_.get(), command_.get(), rtvManager_.get(), barrier_.get(), scissorRect_.get(), viewPort_.get(), fence_.get()); // スワップチェーン
	WinApp::SetSwapChain(swapChain_.get());

	// ポストエフェクトマネージャー(レンダリング関係のマネージャー)
	postEffectManager_->Intialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get(), renderingCommon_.get(), depthStencil_.get(), barrier_.get(), scissorRect_.get(), viewPort_.get());

	imguiManager_->Initialize(this);
}

void Engine::DirectXCommon::Finalize()
{
	imguiManager_->Finalize();
}

void Engine::DirectXCommon::SceneDraw(SceneManager* sceneManager, EntityManager* entity3DManager)
{
	// 描画前処理
	GetSrvManager()->PreDraw();

	// レンダーターゲット用の描画準備
	postEffectManager_->PreDrawOffscreen(); // オフスクリーンのRTV設定

	// 3Dと2D描画
	Draw3D2D(sceneManager, entity3DManager);

	// レンダーターゲット用の描画後処理
	postEffectManager_->PostDrawOffscreen();


	//postEffectManager_->PreDraw2dOffscreen();

	//sceneManager->Draw2D();

	//postEffectManager_->PostDraw2dOffscreen();
}

void Engine::DirectXCommon::PassSwap(SceneManager* sceneManager, RenderTexture* renderTexture)
{
	// スワップチェーン用の描画準備
	swapChain_->PreDraw();

	// レンダーテクスチャ(コピー)
	renderTexture->Draw();

	//sceneManager->Draw2D();

	// ImGuiの描画
	GetImGuiManager()->Draw();

	// スワップチェーン用の描画後処理
	swapChain_->PostDraw();
	postEffectManager_->ClearPostEffectBlock();
	// FPS制限の更新
	UpdateFixFPS();
}

void Engine::DirectXCommon::Draw(SceneManager* sceneManager, EntityManager* entity3DManager)
{
	// 通常描画の前にライト視点の深度を書き出して、影判定に使えるようにする。
	shadowMap_->Update(entity3DManager);
	// インスタンシングとスキニングのシャドウ描画ではSRVを参照するため、通常描画前でもヒープを束縛しておく。
	GetSrvManager()->PreDraw();
	shadowMap_->PreDraw();
	entity3DManager->DrawShadowMap(shadowMap_.get());
	shadowMap_->PostDraw();

	// シーンを書き出す
	SceneDraw(sceneManager, entity3DManager);

	postEffectManager_->AllPostEffect(sceneManager);

	// スワップチェーン
	PassSwap(sceneManager, postEffectManager_->GetEndRenderTexture());
}

void Engine::DirectXCommon::RenderShadowMapDebugImGui()
{
#ifdef _DEBUG
	if (!shadowMap_) {
		return;
	}
	shadowMap_->RenderDebugImGui();
#endif // _DEBUG
}

void Engine::DirectXCommon::Draw3D2D(SceneManager* sceneManager, EntityManager* entity3DManager)
{
	sceneManager->DrawForeground2D();


	entity3DManager->ObjectDraw();

	// 3Dオブジェクトの描画
	sceneManager->Draw3D();



	// パーティクル描画
	entity3DManager->GetEffectManager()->GetParticleManager()->Draw();

	// GPUパーティクル描画
	entity3DManager->GetEffectManager()->GetGpuParticleManager()->Draw();


	entity3DManager->Get3DLineCommon()->Draw();

	// 2Dオブジェクトの描画
	sceneManager->Draw2D();

	entity3DManager->GetEffectManager()->GetParticleManager2d()->Draw();

}

void Engine::DirectXCommon::InitializeFixFPS()
{
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void Engine::DirectXCommon::UpdateFixFPS()
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

void Engine::DirectXCommon::Update(SceneManager* sceneManager, EntityManager* entity3DManager)
{
	// ライト
	entity3DManager->GetLightManager()->Update();

#ifdef _DEBUG
	entity3DManager->UpdateImgui();
#endif // _DEBUG



	sceneManager->Update();

	postEffectManager_->Update(sceneManager->GetCamara());

	entity3DManager->Update();

	entity3DManager->GetEffectManager()->GetParticleManager()->Update();

	entity3DManager->GetEffectManager()->GetGpuParticleManager()->Update();

	//entity3DManager->GetEffectManager()->GetParticleManager2d()->Update();








	entity3DManager->Get3DLineCommon()->Update();

}
